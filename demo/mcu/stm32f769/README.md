
# Porcupine Wake Word Engine Demo for STM32F769

This package contains a demo project for the STM32F769 Discovery kit using Porcupine wake word engine. 

## Installation

For this demo, you need to: 
1. Download and install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), which is an all-in-one multi-OS development tool for STM32 microcontrollers.
2. Install a serial port monitor on your system to be able to communicate with the board. [Arduino environment's built-in serial monitor](https://www.arduino.cc/en/software) and [Coolterm](https://freeware.the-meiers.org/) are two free options available on all platforms (Windows, Linux, and macOS).

## Usage

In order to compile and run the demo project on a STM32f769 discovery board, perform the following steps:

1. Open STM32CubeIDE
2. Click `File` > `Open Projects from file system...` to display the `Import Projects` dialog box. Select the [stm32f769i-disco](./stm32f769i-disco) folder from this repository, and then press the `Finish` button.
3. Select the `stm32f769i-disco-demo` project inside the `Project Explorer` window
4. Click `Project` > `Build All`
5. Connect the board to the computer and press `Run` > `Run`
6. There are two build configurations in this project: Single wake word demo, and Multiple wake words demo; choose one of them in the `Qualifier` window and press `ok`

For the single wake word demo, the default wake word is `Porcupine`; and below are the LED colors associated with supported wake words for the multiple wake words demo:

- ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) `Porcupine`
- ![#ff8000](https://via.placeholder.com/15/ff8000/000000?text=+) `Picovoice`

## Create Custom Wake Word

1. Copy the UUID of the board printed at the beginning of the session to the serial port monitor.
2. Go to [Picovoice Console](https://console.picovoice.ai/) to create a model for [Porcupine wake word engine](https://picovoice.ai/docs/quick-start/console-porcupine/).
3. Select `Arm Cortex-M` as the platform when training the model.
4. Select `STM32` as the board type and provide the UUID of the chipset on the board.

The model is now being trained. You will be able to download it within a few hours.

## Import the Custom Wake Word

1. Download your custom voice model(s) from [Picovoice Console](https://console.picovoice.ai/).
2. Decompress the zip file. The model file is `.ppn` for Porcupine wake word.
3. Use [binary_to_c_array.py](../../../resources/scripts/binary_to_c_array.py) to convert your binary models to C array format  utilizing the following command:
`binary_to_c_array.py --binary_file_path INPUT_PATH --array_file_path OUTPUT_PATH`
1. Copy the content of `output_c_array.txt` and update the `DEFAULT_KEYWORD_ARRAY` value in [/stm32f769i-disco/Inc/pv_params.h](./stm32f769i-disco/Inc/pv_params.h)
 
