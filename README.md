![license](https://img.shields.io/github/license/XLabsProject/iw6x-client.svg)
[![open bugs](https://img.shields.io/github/issues/XLabsProject/iw6x-client/bug?label=bugs)](https://github.com/XLabsProject/iw6x-client/issues?q=is%3Aissue+is%3Aopen+label%3Abug)
[![build](https://github.com/XLabsProject/iw6x-client/workflows/Build/badge.svg)](https://github.com/XLabsProject/iw6x-client/actions)
[![patreon](https://img.shields.io/badge/patreon-support-blue.svg?logo=patreon)](https://www.patreon.com/xlabsproject)
[![discord](https://img.shields.io/endpoint?url=https://momo5502.com/iw4x/members-badge.php)](https://discord.gg/sKeVmR3)


# IW6x: Client

<p align="center">
  <img alig src="assets/github/banner.png?raw=true"/>
</p>

<br/>

## Download

- **[Click here to get the latest release](https://ci.appveyor.com/api/projects/XLabsProject/iw6x-client/artifacts/build%2Fbin%2Fx64%2FRelease%2Fiw6x.exe?branch=master&job=Environment%3A%20APPVEYOR_BUILD_WORKER_IMAGE%3DVisual%20Studio%202019%2C%20PREMAKE_ACTION%3Dvs2019%2C%20CI%3D1%3B%20Configuration%3A%20Release)** (if you are a tester or X Labs staff, get the develop build [here](https://ci.appveyor.com/api/projects/XLabsProject/iw6x-client/artifacts/build%2Fbin%2Fx64%2FRelease%2Fiw6x.exe?branch=develop&job=Environment%3A%20APPVEYOR_BUILD_WORKER_IMAGE%3DVisual%20Studio%202019%2C%20PREMAKE_ACTION%3Dvs2019%2C%20CI%3D1%3B%20Configuration%3A%20Release))
- **You will need to drop this in your Call of Duty: Ghosts installation folder. If you don't have Call of Duty: Ghosts, get those game files first.**
- The client is still in an early stage. It will have bugs!

<br/>

## Showcase

| <img src="https://cdn.discordapp.com/attachments/768362250334765067/773279641540231259/iw6x-release.PNG" /> | <img src="https://pbs.twimg.com/media/EmTRxMJWEAIF6a9?format=jpg&name=large" /> |
|:-:|:-:|
| Server list | Scripting support |

| <img src="https://user-images.githubusercontent.com/45299104/101917488-9721ba00-3bc8-11eb-9a2f-11edeff1dda4.jpg" /> |
|:-:|
| Ultrawide monitor support |

<br/>

## Compile from source

- Clone the Git repo. Do NOT download it as ZIP, that won't work.
- Update the submodules and run `premake5 vs2022` or simply use the delivered `generate.bat`.
- Build via solution file in `build\iw6x.sln`.

### Premake arguments

| Argument                    | Description                                    |
|:----------------------------|:-----------------------------------------------|
| `--copy-to=PATH`            | Optional, copy the EXE to a custom folder after build, define the path here if wanted. |
| `--dev-build`               | Enable development builds of the client. |

<br/>

## Disclaimer

This software has been created purely for the purposes of
academic research. It is not intended to be used to attack
other systems. Project maintainers are not responsible or
liable for misuse of the software. Use responsibly.

