v# murasaki_samples_audio
Audio Sample projects for STM32CubeIDE
## Description
A collection of the Audio sample project for [murasaki](https://github.com/suikan4github/murasaki) project. 

The contents of the repository is a collection of the STM32 CubeIDE projects. Each projects demonstrate the audio application by murasaki project. Recently, there is only one type of audio demonstration application. A talkthrough application which copies line-in audio signal to the headphone-out signal by digital domain. 

## Requirement
- Ubuntu 16.04 LTS
- STM32 CubeIDE v1.1.0
- [Nucleo 144](https://www.st.com/ja/evaluation-tools/stm32-nucleo-boards.html). See below the actual name of the board. 
- [UMB-ADAU1361-A](http://dsps.shop-pro.jp/?pid=82798273) board
- [Akashi-02](https://github.com/suikan4github/Akashi-02) board
## Usage
Following projects are available. 

nucleo-f722-144-akashi02-talkthrough : A project for Nucleo F722ZE. The audio signal to Line-in is copied to HP-out.

nucleo-f746-144-akashi02-talkthrough : A project for Nucleo F746ZG. The audio signal to Line-in is copied to HP-out.

![Nucleo 144 + audio board](img/P_20191125_224443_vHDR_On_HP.jpg)

## Install
1. Install the Egit to CubeIDE by Menu bar -> Help -> Eclipse Marketpalace...
1. Clone [this repository](https://github.com/suikan4github/murasaki_samples_audio.git). Refer [the appropriate section in the Egit documentation](https://wiki.eclipse.org/EGit/User_Guide#Cloning_Remote_Repositories) to clone a repository.
1. Import the audio demo project(s) in to workspace. Refer [the appropriate section in the Egit Documentation](https://wiki.eclipse.org/EGit/User_Guide#Starting_from_existing_Git_Repositories).
1. Build and run
## Licence

[MIT](LICENCE)

## Author

[suikan4github](https://github.com/suikan4github)
