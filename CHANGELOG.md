# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [v2.0.2] - 2021-03-30

### Added

-   Scripting arrays support [#287](https://github.com/XLabsProject/iw6x-client/issues/287)
-   Add unlock-all and god commands [#19](https://github.com/XLabsProject/iw6x-client/issues/19)

### Fixed

-   Cant launch mayday extinction map on iw6x [#319](https://github.com/XLabsProject/iw6x-client/issues/319)

### Pull Requests

-   Scripting arrays #287 [#335](https://github.com/XLabsProject/iw6x-client/pull/335) ([@fedddddd](https://github.com/fedddddd))
-   More script array support [#340](https://github.com/XLabsProject/iw6x-client/pull/340) ([@fedddddd](https://github.com/fedddddd))
-   fix mayday not loading [#349](https://github.com/XLabsProject/iw6x-client/pull/349) ([@FragsAreUs](https://github.com/FragsAreUs))
-   Actual fix for mayday [#355](https://github.com/XLabsProject/iw6x-client/pull/355) ([@OneFourOne](https://github.com/OneFourOne))
-   bring some changes from s1x to iw6x [#367](https://github.com/XLabsProject/iw6x-client/pull/367) ([@iAmThatMichael](https://github.com/iAmThatMichael))

## [v2.0.1] - 2021-01-18

### Fixed

-   2.0.0 server not responding [#315](https://github.com/XLabsProject/iw6x-client/issues/315)

### Pull Requests

-   Fix function table #315 [#316](https://github.com/XLabsProject/iw6x-client/pull/316) ([@fedddddd](https://github.com/fedddddd))

## [v2.0.0] - 2021-01-17

### Fixed

-   Calling a method on wrong entity type crashes the game [#251](https://github.com/XLabsProject/iw6x-client/issues/251)
-   Steam presence not showing [#253](https://github.com/XLabsProject/iw6x-client/issues/253)
-   Function table issue [#254](https://github.com/XLabsProject/iw6x-client/issues/254)
-   Minidump error (Multiplayer) [#262](https://github.com/XLabsProject/iw6x-client/issues/262)
-   Issue with some script API [#264](https://github.com/XLabsProject/iw6x-client/issues/264)
-   linkto function not working [#278](https://github.com/XLabsProject/iw6x-client/issues/278)
-   Jump_height wrong on alien map mp_alien_dlc3 (Awakening) [#284](https://github.com/XLabsProject/iw6x-client/issues/284)
-   Call player function from on_say events [#290](https://github.com/XLabsProject/iw6x-client/issues/290)

### Added

-   Include info about game being clean in minidumps [#248](https://github.com/XLabsProject/iw6x-client/issues/248)
-   Allow executing console commands via scripting API [#255](https://github.com/XLabsProject/iw6x-client/issues/255)
-   Missing lua librarys [#256](https://github.com/XLabsProject/iw6x-client/issues/256)
-   Dedicated Server Port does not change [#106](https://github.com/XLabsProject/iw6x-client/issues/106)
-   Make the require in Lua relative to the script [#259](https://github.com/XLabsProject/iw6x-client/issues/259)
-   Generate cryptographically signed GUIDs and verify them [#263](https://github.com/XLabsProject/iw6x-client/issues/263)

### Pull Requests

-   Ultrawide Support / SafeArea [#242](https://github.com/XLabsProject/iw6x-client/pull/242) ([@xoxor4d](https://github.com/xoxor4d))
-   Fix issue with chat [#249](https://github.com/XLabsProject/iw6x-client/pull/249) ([@fedddddd](https://github.com/fedddddd))
-   fix chat stub so chat actually works [#250](https://github.com/XLabsProject/iw6x-client/pull/250) ([@skiff](https://github.com/skiff))
-   Open table library, fix #256 [#257](https://github.com/XLabsProject/iw6x-client/pull/257) ([@GEEKiDoS](https://github.com/GEEKiDoS))
-   Fix game crash by changing data type from bool to int32 in precache patch [#261](https://github.com/XLabsProject/iw6x-client/pull/261) ([@GEEKiDoS](https://github.com/GEEKiDoS))
-   Fix precaching not working without a fast_restart [#267](https://github.com/XLabsProject/iw6x-client/pull/267) ([@GEEKiDoS](https://github.com/GEEKiDoS))
-   Add g_playerCollision and g_playerEjection [#281](https://github.com/XLabsProject/iw6x-client/pull/281) ([@Joelrau](https://github.com/Joelrau))
-   Update function_dumper [#283](https://github.com/XLabsProject/iw6x-client/pull/283) ([@Joelrau](https://github.com/Joelrau))
-   Add map_restart [#282](https://github.com/XLabsProject/iw6x-client/pull/282) ([@Joelrau](https://github.com/Joelrau))
-   Fix #284 [#285](https://github.com/XLabsProject/iw6x-client/pull/285) ([@fedddddd](https://github.com/fedddddd))

## [v1.1.0] - 2020-12-13

### Fixed

-   Dedicated Server Crash every 24 hours with Fatal error (0xC00000005) at 0x00000001E3A2A0. [#222](https://github.com/XLabsProject/iw6x-client/issues/222)
-   SetThreadDescription not available on older Windows versions [#237](https://github.com/XLabsProject/iw6x-client/issues/237)
-   Crash after leaving lobby (Local Match/Private) [#239](https://github.com/XLabsProject/iw6x-client/issues/239)
-   jump_slowDownEnable animation bug [#245](https://github.com/XLabsProject/iw6x-client/issues/245)

### Added

-   Implement scripting support [#17](https://github.com/XLabsProject/iw6x-client/issues/17)

### Pull Requests

-   Allow server owner to set server port [#197](https://github.com/XLabsProject/iw6x-client/pull/197) ([@GEEKiDoS](https://github.com/GEEKiDoS))
-   Add chat notifies [#235](https://github.com/XLabsProject/iw6x-client/pull/235) ([@fedddddd](https://github.com/fedddddd))
-   Implement some jump dvars [#243](https://github.com/XLabsProject/iw6x-client/pull/243) ([@fedddddd](https://github.com/fedddddd))
-   Fix #245 [#246](https://github.com/XLabsProject/iw6x-client/pull/246) ([@fedddddd](https://github.com/fedddddd))

## [v1.0.5] - 2020-12-08

### Fixed

-   Singleplayer crashing with v1.0.4 [#234](https://github.com/XLabsProject/iw6x-client/issues/234)

## [v1.0.4] - 2020-12-07

### Fixed

-   Game pad crashes due to missing steam-api interfaces [#173](https://github.com/XLabsProject/iw6x-client/issues/173)

### Added

-   Analyze Arxan exception handlers to get rid of them [#146](https://github.com/XLabsProject/iw6x-client/issues/146)
-   Add proper support for fovs >= 90 [#153](https://github.com/XLabsProject/iw6x-client/issues/153)
-   Set thread names for common threads [#226](https://github.com/XLabsProject/iw6x-client/issues/226)
-   Add -no-steam parameter to disable steam integration [#227](https://github.com/XLabsProject/iw6x-client/issues/227)
-   Create extended minidumps [#228](https://github.com/XLabsProject/iw6x-client/issues/228)
-   Load game as library instead of injecting it [#230](https://github.com/XLabsProject/iw6x-client/issues/230)

### Pull Requests

-   added r_fullbright, kbam input patch and changed branding in sp [#229](https://github.com/XLabsProject/iw6x-client/pull/229) ([@Jemal](https://github.com/Jemal))
-   allow PlayerCmd_SetClientDvar to change any dvar like old cods [#224](https://github.com/XLabsProject/iw6x-client/pull/224) ([@skiff](https://github.com/skiff))
-   Update function_table and some other changes. [#232](https://github.com/XLabsProject/iw6x-client/pull/232) ([@Joelrau](https://github.com/Joelrau))

## [v1.0.3] - 2020-11-25

### Fixed

-   Dedicated server incompatible with older Windows [#167](https://github.com/XLabsProject/iw6x-client/issues/167)
-   Fatal Error Server Crash [#119](https://github.com/XLabsProject/iw6x-client/issues/119)

### Pull Requests

-   Implement sv_motd [#203](https://github.com/XLabsProject/iw6x-client/pull/203) ([@fedddddd](https://github.com/fedddddd))
-   MP command fixes, Game console version, Aim assist [#208](https://github.com/XLabsProject/iw6x-client/pull/208) ([@skiff](https://github.com/skiff))
-   Add fastfiles component [#199](https://github.com/XLabsProject/iw6x-client/pull/199) ([@Joelrau](https://github.com/Joelrau))
-   update give command & add take command [#198](https://github.com/XLabsProject/iw6x-client/pull/198) ([@Joelrau](https://github.com/Joelrau))

## [v1.0.2] - 2020-11-12

### Fixed

-   SP crashes on mouse scroll [#186](https://github.com/XLabsProject/iw6x-client/issues/186)

### Pull Requests

-   Change steam_id generation to a more suitable random [#178](https://github.com/XLabsProject/iw6x-client/pull/178) ([@skiff](https://github.com/skiff))

## [v1.0.1] - 2020-11-05

### Pull Requests

-   Fix chat font on resolutions higher than 1080p [#176](https://github.com/XLabsProject/iw6x-client/pull/176) ([@skiff](https://github.com/skiff))
-   Show infomation on title of the dedicated console [#166](https://github.com/XLabsProject/iw6x-client/pull/166) ([@GEEKiDoS](https://github.com/GEEKiDoS))

## [v1.0.0] - 2020-11-03

### Fixed

-   2nd octet of the IP-address in status response is wrong. [#150](https://github.com/XLabsProject/iw6x-client/issues/150)
-   Arxan exceptions cause performance issues [#144](https://github.com/XLabsProject/iw6x-client/issues/144)

### Added

-   Disable whitelist protection on /bind command on console. [#151](https://github.com/XLabsProject/iw6x-client/issues/151)

### Pull Requests

-   ScrCmd_SetSlowMotion patches [#157](https://github.com/XLabsProject/iw6x-client/pull/157) ([@skiff](https://github.com/skiff))
-   added binding [#152](https://github.com/XLabsProject/iw6x-client/pull/152) ([@Jemal](https://github.com/Jemal))
-   Server double clicks, discord presence fix and input module [#148](https://github.com/XLabsProject/iw6x-client/pull/148) ([@Jemal](https://github.com/Jemal))
-   Add clientkick and kick commands [#147](https://github.com/XLabsProject/iw6x-client/pull/147) ([@Joelrau](https://github.com/Joelrau))

## [v1.0.0-rc.1] - 2020-10-28

### Fixed

-   Dedis sometimes crash at 0x28046F420 [#104](https://github.com/XLabsProject/iw6x-client/issues/104)
-   A very long sv_hostname creates a lot of chaos [#102](https://github.com/XLabsProject/iw6x-client/issues/102)
-   Crash while typing message in chat [#99](https://github.com/XLabsProject/iw6x-client/issues/99)
-   steam_api64 is still being loaded by the game [#95](https://github.com/XLabsProject/iw6x-client/issues/95)
-   Discord integration displays own hostname, not the remote one [#94](https://github.com/XLabsProject/iw6x-client/issues/94)
-   Manage connecting from the main menu [#93](https://github.com/XLabsProject/iw6x-client/issues/93)
-   Changing password in private match crashes [#80](https://github.com/XLabsProject/iw6x-client/issues/80)
-   Exec bugged [#76](https://github.com/XLabsProject/iw6x-client/issues/76)
-   Dedicated server takes up a player slot [#75](https://github.com/XLabsProject/iw6x-client/issues/75)
-   Dedicated servers lag/run at double speed [#67](https://github.com/XLabsProject/iw6x-client/issues/67)
-   9-bangs and semtex's are bugged (Desc) [#39](https://github.com/XLabsProject/iw6x-client/issues/39)
-   New bounce command bugged [#25](https://github.com/XLabsProject/iw6x-client/issues/25)
-   Vsync 15FPS bug [#22](https://github.com/XLabsProject/iw6x-client/issues/22)
-   Ingame console is automatically opened in singleplayer [#7](https://github.com/XLabsProject/iw6x-client/issues/7)

### Added

-   Delay dedicated server commands until initialization is done [#98](https://github.com/XLabsProject/iw6x-client/issues/98)
-   Verify the game binary versions [#92](https://github.com/XLabsProject/iw6x-client/issues/92)
-   Add rcon and status commands [#91](https://github.com/XLabsProject/iw6x-client/issues/91)
-   Dealing with LFS limitations [#86](https://github.com/XLabsProject/iw6x-client/issues/86)
-   Load dedicated server configs [#85](https://github.com/XLabsProject/iw6x-client/issues/85)
-   Implement map rotate for dedis [#81](https://github.com/XLabsProject/iw6x-client/issues/81)
-   Dedicated Servers load .pak files [#71](https://github.com/XLabsProject/iw6x-client/issues/71)
-   Implement exec mycustom.cfg [#66](https://github.com/XLabsProject/iw6x-client/issues/66)
-   Implement basic server backend [#60](https://github.com/XLabsProject/iw6x-client/issues/60)
-   Implement minidump support [#59](https://github.com/XLabsProject/iw6x-client/issues/59)
-   Block changing name ingame [#57](https://github.com/XLabsProject/iw6x-client/issues/57)
-   Remove name colors from overhead names [#56](https://github.com/XLabsProject/iw6x-client/issues/56)
-   Implement dedicated servers [#29](https://github.com/XLabsProject/iw6x-client/issues/29)
-   Add working connect command [#28](https://github.com/XLabsProject/iw6x-client/issues/28)
-   Remove DTLS [#27](https://github.com/XLabsProject/iw6x-client/issues/27)
-   Get basic matchmaking working [#9](https://github.com/XLabsProject/iw6x-client/issues/9)
-   Dump latest publisher files [#8](https://github.com/XLabsProject/iw6x-client/issues/8)
-   Wai you no fully remove the most basic version of Arxan?!one! [#1](https://github.com/XLabsProject/iw6x-client/issues/1)

### Pull Requests

-   Add more dvars to require sv_cheats / cleaned pathes a little [#120](https://github.com/XLabsProject/iw6x-client/pull/120) ([@FragsAreUs](https://github.com/FragsAreUs))
-   Implement release process automation [#118](https://github.com/XLabsProject/iw6x-client/pull/118) ([@x-dev-urandom](https://github.com/x-dev-urandom))
-   Add IW6x version to game version strings [#116](https://github.com/XLabsProject/iw6x-client/pull/116) ([@x-dev-urandom](https://github.com/x-dev-urandom))
-   dvarDump show values & using enums for game_console::print [#114](https://github.com/XLabsProject/iw6x-client/pull/114) ([@iAmThatMichael](https://github.com/iAmThatMichael))
-   Add some functions [#110](https://github.com/XLabsProject/iw6x-client/pull/110) ([@Joelrau](https://github.com/Joelrau))
-   Unlock all camos [#108](https://github.com/XLabsProject/iw6x-client/pull/108) ([@zzzcarter](https://github.com/zzzcarter))
-   Added a simple ping counter [#105](https://github.com/XLabsProject/iw6x-client/pull/105) ([@halcyo-n](https://github.com/halcyo-n))
-   Display the proper hostname in discord presence [#103](https://github.com/XLabsProject/iw6x-client/pull/103) ([@Jemal](https://github.com/Jemal))
-   Patch Dvar_Command to print values if no values are given like CoD4 [#97](https://github.com/XLabsProject/iw6x-client/pull/97) ([@skiff](https://github.com/skiff))
-   rcon and status commands [#96](https://github.com/XLabsProject/iw6x-client/pull/96) ([@skiff](https://github.com/skiff))
-   Updated DPServer IP Address [#90](https://github.com/XLabsProject/iw6x-client/pull/90) ([@HexChaos](https://github.com/HexChaos))
-   Hook calls for cg_gun\_ dvars to remove flags [#89](https://github.com/XLabsProject/iw6x-client/pull/89) ([@skiff](https://github.com/skiff))
-   Fix map_rotation crash [#88](https://github.com/XLabsProject/iw6x-client/pull/88) ([@Joelrau](https://github.com/Joelrau))
-   Set up GitHub Actions [#87](https://github.com/XLabsProject/iw6x-client/pull/87) ([@x-dev-urandom](https://github.com/x-dev-urandom))
-   New artworks & resources [#84](https://github.com/XLabsProject/iw6x-client/pull/84) ([@sortileges](https://github.com/sortileges))
-   Implement sv_cheats [#82](https://github.com/XLabsProject/iw6x-client/pull/82) ([@skiff](https://github.com/skiff))
-   Small changes [#78](https://github.com/XLabsProject/iw6x-client/pull/78) ([@Joelrau](https://github.com/Joelrau))
-   Fix exec command patch [#77](https://github.com/XLabsProject/iw6x-client/pull/77) ([@skiff](https://github.com/skiff))
-   New icon, splash screen and banner [#70](https://github.com/XLabsProject/iw6x-client/pull/70) ([@sortileges](https://github.com/sortileges))
-   Allow exec to find custom cfg files on disk [#69](https://github.com/XLabsProject/iw6x-client/pull/69) ([@skiff](https://github.com/skiff))
-   Increased sv_max_fps to 1000 [#68](https://github.com/XLabsProject/iw6x-client/pull/68) ([@xerxes-at](https://github.com/xerxes-at))
-   Add Com_Error function [#64](https://github.com/XLabsProject/iw6x-client/pull/64) ([@Joelrau](https://github.com/Joelrau))
-   Block changing name in-game & remove colors from overhead names [#63](https://github.com/XLabsProject/iw6x-client/pull/63) ([@Jemal](https://github.com/Jemal))
-   "cg_gun\_" and dvar patches [#62](https://github.com/XLabsProject/iw6x-client/pull/62) ([@sortileges](https://github.com/sortileges))
-   cg_fovscale and cg_fov changes [#58](https://github.com/XLabsProject/iw6x-client/pull/58) ([@lizardpeter](https://github.com/lizardpeter))
-   Added bots [#55](https://github.com/XLabsProject/iw6x-client/pull/55) ([@Jemal](https://github.com/Jemal))
-   Patched r_fog [#53](https://github.com/XLabsProject/iw6x-client/pull/53) ([@OneFourOne](https://github.com/OneFourOne))
-   Discord integration [#52](https://github.com/XLabsProject/iw6x-client/pull/52) ([@Jemal](https://github.com/Jemal))
-   Added colors module [#48](https://github.com/XLabsProject/iw6x-client/pull/48) ([@Jemal](https://github.com/Jemal))
-   Scheduler addons [#47](https://github.com/XLabsProject/iw6x-client/pull/47) ([@skiff](https://github.com/skiff))
-   Fix SP not starting [#46](https://github.com/XLabsProject/iw6x-client/pull/46) ([@fedddddd](https://github.com/fedddddd))
-   Implemented server list feeder [#43](https://github.com/XLabsProject/iw6x-client/pull/43) ([@Jemal](https://github.com/Jemal))
-   Add setviewpos, setviewang commands, add viewpos to drawfps [#38](https://github.com/XLabsProject/iw6x-client/pull/38) ([@fedddddd](https://github.com/fedddddd))
-   Make stats look better [#36](https://github.com/XLabsProject/iw6x-client/pull/36) ([@FragsAreUs](https://github.com/FragsAreUs))

[Unreleased]: https://github.com/XLabsProject/iw6x-client/compare/v2.0.2...HEAD

[v2.0.2]: https://github.com/XLabsProject/iw6x-client/compare/v2.0.1...v2.0.2

[v2.0.1]: https://github.com/XLabsProject/iw6x-client/compare/v2.0.0...v2.0.1

[v2.0.0]: https://github.com/XLabsProject/iw6x-client/compare/v1.1.0...v2.0.0

[v2.0.0]: https://github.com/XLabsProject/iw6x-client/compare/v1.1.0...v2.0.0

[v1.1.0]: https://github.com/XLabsProject/iw6x-client/compare/v1.0.5...v1.1.0

[v1.0.5]: https://github.com/XLabsProject/iw6x-client/compare/v1.0.4...v1.0.5

[v1.0.4]: https://github.com/XLabsProject/iw6x-client/compare/v1.0.3...v1.0.4

[v1.0.3]: https://github.com/XLabsProject/iw6x-client/compare/v1.0.2...v1.0.3

[v1.0.2]: https://github.com/XLabsProject/iw6x-client/compare/v1.0.1...v1.0.2

[v1.0.1]: https://github.com/XLabsProject/iw6x-client/compare/v1.0.0...v1.0.1

[v1.0.0]: https://github.com/XLabsProject/iw6x-client/compare/v1.0.0-rc.1...v1.0.0

[v1.0.0-rc.1]: https://github.com/XLabsProject/iw6x-client/compare/22c834e0655795870621ce505ea189ae522e8223...v1.0.0-rc.1
