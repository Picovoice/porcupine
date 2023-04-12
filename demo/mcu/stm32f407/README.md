# Porcupine Wake Word Engine Demo for STM32F407 (Multiple languages)

This package contains a demo project for the STM32F407 Discovery kit using Porcupine wake word engine.

## Supported Languages

1. Arabic
2. Dutch
3. English
4. French
5. German
6. Hindi
7. Italian
8. Japanese
9. Korean
10. Mandarin
11. Polish
12. Portuguese
13. Russian
14. Spanish
15. Swedish
16. Vietnamese

- Support for additional languages is available for commercial customers on a case-by-case basis.


## Installation

For this demo, you need to:

1. Download and install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), which is an
   all-in-one multi-OS development tool for STM32 microcontrollers.
2. Download [STM32Cube MCU Package for STM32F4 series](https://www.st.com/en/embedded-software/stm32cubef4.html) and
   extract it somewhere on your computer.

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using
Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

In the demo project, there is a separate build configuration for each supported languages. In order to activate a
specific configuration:

1. Click `Project` > `Build Configuration` > `Set Active`
2. Select the target configuration

Then, to compile and run the demo project on a STM32f407 discovery board, perform the following steps:

1. Open STM32CubeIDE
2. Click `File` > `Open Projects from file system...` to display the `Import Projects` dialog box. Select
   the [stm32f407g-disc1](./stm32f407g-disc1) folder from this repository, and then press the `Finish` button.
3. Go to the folder where you extracted `STM32Cube MCU Package for STM32F4 series`, and then copy the contents of
   the `/Middlewares/ST/STM32_Audio/Addons/PDM` folder
   to [/Middlewares/ST/STM32_Audio/Addons/PDM](./stm32f407g-disc1/Middlewares/ST/STM32_Audio/Addons/PDM).
4. Select the `stm32f407g-disc1-demo` project inside the `Project Explorer` window
5. Replace `ACCESS_KEY` in both `main.c` and `main_multi.c` with your AccessKey obtained
   from [Picovoice Console](https://console.picovoice.ai/)
6. Click `Project` > `Build Project`
7. Connect the board to the computer and press `Run` > `Debug`
8. There are two build configurations in this project: Single wake word demo, and Multiple wake words demo; choose one
   of them in the `Qualifier` window and press `ok`

> :warning: `printf()` uses the SWO connector and the trace port 0. For more information, refer
> to [STM32 microcontroller debug toolbox](https://www.st.com/resource/en/application_note/dm00354244-stm32-microcontroller-debug-toolbox-stmicroelectronics.pdf)
> , Chapter 7.

For single wake-word demos, you can identify the default keyword for each language by referring to the [pv_params.h](./stm32f407g-disc1/Inc/pv_params.h) file. Within this file, locate the language section enclosed by:

```c
#if defined(__PV_LANGUAGE_{LANGUAGE_NAME}__)
...
#endif
```

The default keyword for each language will be listed next to the `// wake-word` comment.

Below are the LED colors associated with supported wake words for the multiple wake words demo:
<!-- markdown-link-check-disable -->

- ![#00ff00](../../../resources/.images/00ff00.png) `Porcupine`
- ![#ff8000](../../../resources/.images/ff8000.png) `Picovoice`
- ![#ff0000](../../../resources/.images/ff0000.png) `Bumblebee`
- ![#0000ff](../../../resources/.images/0000ff.png) `Alexa`

<!-- markdown-link-check-enable -->

## Create Custom Wake Word

1. Copy the UUID of the board printed at the beginning of the session to the serial port monitor.
2. Go to [Picovoice Console](https://console.picovoice.ai/) to create a model
   for [Porcupine wake word engine](https://picovoice.ai/docs/quick-start/console-porcupine/).
3. Select `Arm Cortex-M` as the platform when training the model.
4. Select `STM32` as the board type and provide the UUID of the chipset on the board.

The model is now being trained. You will be able to download it within a few hours.

## Import the Custom Wake Word

1. Download your custom voice model(s) from [Picovoice Console](https://console.picovoice.ai/).
2. Decompress the zip file. The model for Porcupine wake word is located in two files: A binary `.ppn` file, and as
   a `.h` header file containing a `C` array version of the binary model.
3. Copy the contents of the array inside the `.h` header file and update the `DEFAULT_KEYWORD_ARRAY` value
   in [/stm32f407g-disc1/Inc/pv_params.h](./stm32f407g-disc1/Inc/pv_params.h) in the language section for which the
   model is trained.
