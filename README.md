![license](https://img.shields.io/github/license/CBServers/iw6x-client.svg)
[![build](https://img.shields.io/github/actions/workflow/status/CBServers/iw6x-client/build.yml?branch=main&label=Build&logo=github)](https://github.com/CBServers/iw6x-client/actions)
[![bugs](https://img.shields.io/github/issues/CBServers/iw6x-client/bug?label=Bugs)](https://github.com/CBServers/iw6x-client/issues?q=is%3Aissue+is%3Aopen+label%3Abug)
[![website](https://img.shields.io/badge/CBServers-Website-blue)](https://cbservers.xyz)


# IW6x: Client
This is a fork of IW6x/[iw6-mod](https://git.alterware.dev/AlterWare/iw6-mod)

Originally developed by [AlterWare](https://alterware.dev) and [X Labs](https://xlabs.dev/).

Thanks to the all orginal contributors.

<p align="center">
  <img alig src="assets/github/banner.png?raw=true"/>
</p>

## Download

- **[Click here to get the latest release](https://github.com/CBServers/updater/raw/main/updater/iw6x/iw6x.exe)**
- **You will need to drop this in your Call of Duty: Ghosts installation folder. If you don't have Call of Duty: Ghosts, get those game files first.**
- The client is still in an early stage. It will have bugs!

## Compile from source

- Clone the Git repo. Do NOT download it as ZIP, that won't work.
- Update the submodules and run `premake5 vs2022` or simply use the delivered `generate.bat`.
- Build via solution file in `build\iw6x.sln`.

### Premake arguments

| Argument                    | Description                                    |
|:----------------------------|:-----------------------------------------------|
| `--copy-to=PATH`            | Optional, copy the EXE to a custom folder after build, define the path here if wanted. |
| `--dev-build`               | Enable development builds of the client. |

## Disclaimer

This software has been created purely for the purposes of
academic research. It is not intended to be used to attack
other systems. Project maintainers are not responsible or
liable for misuse of the software. Use responsibly.