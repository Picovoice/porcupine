# Porcupine Wake Word Engine Demo for IMXRT1050-EVKB

This package contains a demo project for the i.MX RT1050 Evaluation Kit using Porcupine wake word engine.

## Installation

For this demo, you need to download and install [MCUXpresso IDE](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE), which is an all-in-one multi-OS development tool for NXP MCUs based on Arm Cortex-M cores.

## Usage

In order to compile and run the demo project on a i.MX RT1050 Evaluation board, perform the following steps:

1. Open `MCUXpresso IDE`
2. From the main toolbar, select `Install MCUXpresso SDKs` and install `evkbimxrt1050` SDK
3. Click `File` > `Open Projects from file system...` to display the `Import Projects` dialog box.Select the [imxrt1050-evkb](./imxrt1050-evkb) folder from this repository, and then press the `Finish` button.
4. Click `Project` > `Build All`
5. Connect the board to the computer, select the imported project inside `Project Explorer` window
6. Click `Run` > `Debug as` > `MCUXpresso IDE LinkServer probes` and then select the connected board.
7. In the debug view, press `Run` > `Resume`

For the single wake word demo, the default wake word is `Porcupine`; and for the multiple wake words demo `Porcupine`, `Picovoice`, `Bumblebee`, and `Alexa` are considered as the keywords.


## Create Custom Models

1. Copy the UUID of the board printed at the beginning of the session to the IDE console.
1. Go to [Picovoice Console](https://console.picovoice.ai/) to create a model for [Porcupine wake word engine](https://picovoice.ai/docs/quick-start/console-porcupine/).
1. Select `Arm Cortex-M` as the platform when training the model.
1. Select `IMXRT` as the board type and provide the UUID of the chipset on the board.

The model is now being trained. You will be able to download it within a few hours.

## Import the Custom Models

1. Download your custom voice model(s) from [Picovoice Console](https://console.picovoice.ai/).
1. Decompress the zip file. The model file is `.ppn` for Porcupine wake word.
1. Use [binary_to_c_array.py](../../../resources/scripts/binary_to_c_array.py) to convert your binary models to C array format  utilizing the following command:
`python3 binary_to_c_array.py input_binary_model output_c_array.txt`
1. Copy the content of `output_c_array.txt` and update the `DEFAULT_KEYWORD_ARRAY` and `context_array` values in [imxrt1050-evkb/inc/pv_params.h](./imxrt1050-evkb/inc/pv_params.h).
 
