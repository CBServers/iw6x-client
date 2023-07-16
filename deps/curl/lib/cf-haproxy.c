/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/

#include "curl_setup.h"

#if !defined(CURL_DISABLE_PROXY)

#include <curl/curl.h>
#include "urldata.h"
#include "cfilters.h"
#include "cf-haproxy.h"
#include "curl_log.h"
#include "multiif.h"

/* The last 3 #include files should be in this order */
#include "curl_printf.h"
#include "curl_memory.h"
#include "memdebug.h"


typedef enum {
    HAPROXY_INIT,     /* init/default/no tunnel state */
    HAPROXY_SEND,     /* data_out being sent */
    HAPROXY_DONE      /* all work done */
} haproxy_state;

struct cf_haproxy_ctx {
  int state;
  struct dynbuf data_out;
};

static void cf_haproxy_ctx_reset(struct cf_haproxy_ctx *ctx)
{
  DEBUGASSERT(ctx);
  ctx->state = HAPROXY_INIT;
  Curl_dyn_reset(&ctx->data_out);
}

static void cf_haproxy_ctx_free(struct cf_haproxy_ctx *ctx)
{
  if(ctx) {
    Curl_dyn_free(&ctx->data_out);
    free(ctx);
  }
}

static CURLcode cf_haproxy_date_out_set(struct Curl_cfilter*cf,
                                        struct Curl_easy *data)
{
  struct cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  const char *tcp_version;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  if(cf->conn->unix_domain_socket)
    /* the buffer is large enough to hold this! */
    result = Curl_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  else {
#endif /* USE_UNIX_SOCKETS */
  /* Emit the correct prefix for IPv6 */
  tcp_version = cf->conn->bits.ipv6 ? "TCP6" : "TCP4";

  result = Curl_dyn_addf(&ctx->data_out, "PROXY %s %s %s %i %i\r\n",
                         tcp_version,
                         data->info.conn_local_ip,
                         data->info.conn_primary_ip,
                         data->info.conn_local_port,
                         data->info.conn_primary_port);

#ifdef USE_UNIX_SOCKETS
  }
#endif /* USE_UNIX_SOCKETS */
  return result;
}

static CURLcode cf_haproxy_connect(struct Curl_cfilter *cf,
                                   struct Curl_easy *data,
                                   bool blocking, bool *done)
{
  struct cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  if(cf->connected) {
    *done = TRUE;
    return CURLE_OK;
  }

  result = cf->next->cft->connect(cf->next, data, blocking, done);
  if(result || !*done)
    return result;

  switch(ctx->state) {
  case HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    if(result)
      goto out;
    ctx->state = HAPROXY_SEND;
    /* FALLTHROUGH */
  case HAPROXY_SEND:
    len = Curl_dyn_len(&ctx->data_out);
    if(len > 0) {
      ssize_t written = Curl_conn_send(data, cf->sockindex,
                                       Curl_dyn_ptr(&ctx->data_out),
                                       len, &result);
      if(written < 0)
        goto out;
      Curl_dyn_tail(&ctx->data_out, len - (size_t)written);
      if(Curl_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        goto out;
      }
    }
    ctx->state = HAPROXY_DONE;
    /* FALLTHROUGH */
  default:
    Curl_dyn_free(&ctx->data_out);
    break;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  return result;
}

static void cf_haproxy_destroy(struct Curl_cfilter *cf,
                               struct Curl_easy *data)
{
  (void)data;
  DEBUGF(LOG_CF(data, cf, "destroy"));
  cf_haproxy_ctx_free(cf->ctx);
}

static void cf_haproxy_close(struct Curl_cfilter *cf,
                             struct Curl_easy *data)
{
  DEBUGF(LOG_CF(data, cf, "close"));
  cf->connected = FALSE;
  cf_haproxy_ctx_reset(cf->ctx);
  if(cf->next)
    cf->next->cft->close(cf->next, data);
}

static int cf_haproxy_get_select_socks(struct Curl_cfilter *cf,
                                       struct Curl_easy *data,
                                       curl_socket_t *socks)
{
  int fds;

  fds = cf->next->cft->get_select_socks(cf->next, data, socks);
  if(!fds && cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    socks[0] = Curl_conn_cf_get_socket(cf, data);
    return GETSOCK_WRITESOCK(0);
  }
  return fds;
}


struct Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  0,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  cf_haproxy_close,
  Curl_cf_def_get_host,
  cf_haproxy_get_select_socks,
  Curl_cf_def_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

static CURLcode cf_haproxy_create(struct Curl_cfilter **pcf,
                                  struct Curl_easy *data)
{
  struct Curl_cfilter *cf = NULL;
  struct cf_haproxy_ctx *ctx;
  CURLcode result;

  (void)data;
  ctx = calloc(sizeof(*ctx), 1);
  if(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  ctx->state = HAPROXY_INIT;
  Curl_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  if(result)
    goto out;
  ctx = NULL;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result? NULL : cf;
  return result;
}

CURLcode Curl_conn_haproxy_add(struct Curl_easy *data,
                               struct connectdata *conn,
                               int sockindex)
{
  struct Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  if(result)
    goto out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  return result;
}

CURLcode Curl_cf_haproxy_insert_after(struct Curl_cfilter *cf_at,
                                      struct Curl_easy *data)
{
  struct Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  if(result)
    goto out;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  return result;
}

#endif /* !CURL_DISABLE_PROXY */
