# Porcupine Wake Word Engine Demo for STM32H747 (Multiple languages)

This package contains a demo project for the STM32H747 Discovery kit using Porcupine wake word engine. 

## Supported Languages

1. English
2. French
3. German
4. Spanish

## Installation

For this demo, you need to: 
1. Download and install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), which is an all-in-one multi-OS development tool for STM32 microcontrollers.
1. Download `STM32Cube middleware for audio PDM to PCM conversion` and copy it to the project folder. A more detailed guide can be found in the [readme](./stm32h747i-disco/CM7/Middlewares/ST/STM32_Audio/Addons/PDM/readme.txt) file.
1. Install a serial port monitor on your system to be able to communicate with the board. [Arduino environment's built-in serial monitor](https://www.arduino.cc/en/software) and [Coolterm](https://freeware.the-meiers.org/) are two free options available on all platforms (Windows, Linux, and macOS).

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

In the demo project, there is a separate build configuration for each supported languages. In order to activate a specific configuration:

1. Click `Project` > `Build Configuration` > `Set Active`
2. Select the target configuration

Then, to compile and run the demo project on a STM32H747 discovery board, perform the following steps:

1. Open STM32CubeIDE
2. Click `File` > `Open Projects from file system...` to display the `Import Projects` dialog box. Select the [stm32h747i-disco](./stm32h747i-disco) folder from this repository, and then press the `Finish` button.
3. Copy the `Inc` and `Lib` folders from the downloaded **PCM2PDM** library to [/CM7/Middlewares/ST/STM32_Audio/Addons/PDM](./stm32h747i-disco/CM7/Middlewares/ST/STM32_Audio/Addons/PDM)
4. Select the `stm32h747i-disco-demo_CM7` inner project inside the `Project Explorer` window
5. Replace `ACCESS_KEY` in both `main.c` and `main_multi.c` with your AccessKey obtained from [Picovoice Console](https://console.picovoice.ai/)
5. Click `Project` > `Build Project`
6. Connect the board to the computer and press `Run` > `Run`
7. There are two build configurations in this project: Single wake word demo, and Multiple wake words demo; choose one of them in the `Qualifier` window and press `ok`

For the single wake word demos, the default wake words are:

- `Porcupine` for English language,
- `salut ordinateur` for French language.
- `hey computer` for German language,
- `hola computadora` for Spanish language,

Below are the LED colors associated with supported wake words for the multiple wake words demo:
<!-- markdown-link-check-disable -->
- ![#00ff00](../../../resources/images/00ff00.png) `Porcupine`
- ![#ff8000](../../../resources/images/ff8000.png) `Picovoice`
- ![#ff0000](../../../resources/images/ff0000.png) `Bumblebee`
- ![#0000ff](../../../resources/images/0000ff.png) `Alexa`
<!-- markdown-link-check-enable -->
> :warning: **This project exclusively utilizes the Cortex-M7 core of the STM32H747XIH6 microcontroller. If you would like to use the Cortex-M4 core along with the Cortex-M7 core, set the BOOT_CM4_ADD0 option byte in STM32CubeProgrammer to 0x818**

## Create Custom Wake Word

1. Copy the UUID of the board printed at the beginning of the session to the serial port monitor.
2. Go to [Picovoice Console](https://console.picovoice.ai/) to create a model for [Porcupine wake word engine](https://picovoice.ai/docs/quick-start/console-porcupine/).
3. Select `Arm Cortex-M` as the platform when training the model.
4. Select `STM32` as the board type and provide the UUID of the chipset on the board.

The model is now being trained. You will be able to download it within a few hours.

## Import the Custom Wake Word

1. Download your custom voice model(s) from [Picovoice Console](https://console.picovoice.ai/).
2. Decompress the zip file. The model for Porcupine wake word is located in two files: A binary `.ppn` file, and as a `.h` header file containing a `C` array version of the binary model.
3. Copy the contents of the array inside the `.h` header file and update the `DEFAULT_KEYWORD_ARRAY` value in [/stm32h747i-disco/CM7/Inc/pv_params.h](./stm32h747i-disco/CM7/Inc/pv_params.h) in the language section for which the model is trained.
