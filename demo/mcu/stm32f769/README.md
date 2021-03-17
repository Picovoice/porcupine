
# Picovoice STM32F769I-DISCO Demo

This package contains a demo project for the STM32F769 Discovery kit using Picovoice platform.

## Installation

For this demo, you need to: 
1. Download and install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), which is an all-in-one multi-OS development tool for STM32 microcontrollers.
1. Install a serial port monitor on your system to be able to communicate with the board. [Arduino environment's built-in serial monitor](https://www.arduino.cc/en/software) and [Coolterm](https://freeware.the-meiers.org/) are two free options available on all platforms (Windows, Linux, and macOS).

## Usage

In order to compile and run the demo project on a STM32F769 discovery board, perform the following steps:

1. Open STM32CubeIDE
1. Click `File` > `Open Projects from file system...` to display the `Import Projects` dialog box. Select the [stm32f769i-disco](./stm32f769i-disco) folder from this repository, and then press the `Finish` button.
1. Click `Project` > `Build All`
1. Connect the board to the computer and press `Run` > `Run`

For this demo, the default wake word is `Picovoice` and the context is `Smart Lighting`. The engine can recognize commands such as

> Picovoice, turn off the lights.

or

> Picovoice, set the lights in the bedroom to blue.

Picovoice's output can be seen on the serial port monitor.

See below for the full context:

```yaml
context:
  expressions:
    changeColor:
      - "[turn, make] (all, the) lights $color:color"
      - "[change, set, switch] (all, the) lights to $color:color"
      - "[turn, make] (the) $location:location (color, light, lights) $color:color"
      - "[change, set, switch] (the) $location:location (color, light, lights) to $color:color"
      - "[turn, make] (the) [color, light, lights] [at, in] (the) $location:location $color:color"
      - "[change, set, switch] (the) [color, light, lights] [at, in] (the) $location:location to $color:color"
      - "[turn, make] (the) [color, light, lights] $color:color [at, in] (the) $location:location"
      - "[change, set, switch] (the) [color, light, lights] to $color:color [at, in] (the) $location:location"
    changeLightState:
      - "[switch, turn] $state:state (all, the) lights"
      - "[switch, turn] (all, the) lights $state:state"
      - "[switch, turn] $state:state (the) $location:location (light, lights)"
      - "[switch, turn] (the) $location:location [light, lights] $state:state"
      - "[switch, turn] $state:state (the) [light, lights] [at, in] (the) $location:location"
      - "[switch, turn] (the) [light, lights] [in, at] the $location:location $state:state"
    changeLightStateOff:
      - "shut off (all, the) lights"
      - "shut (all, the) lights off"
      - "shut off (the) $location:location (light, lights)"
      - "shut (the) $location:location (light, lights) off"
      - "shut off (the) [light, lights] [at, in] (the) $location:location"
      - "shut (the) [light, lights] off [at, in] (the) $location:location"
      - "shut (the) [light, lights] [at, in] (the) $location:location off"
  slots:
    color:
      - "blue"
      - "green"
      - "orange"
      - "pink"
      - "purple"
      - "red"
      - "white"
      - "yellow"
    state:
      - "off"
      - "on"
    location:
      - "bathroom"
      - "bedroom"
      - "closet"
      - "hallway"
      - "kitchen"
      - "living room"
      - "pantry"
```
## Create Custom Models

1. Copy the UUID of the board printed at the beginning of the session to the serial port monitor.
1. Go to [Picovoice Console](https://console.picovoice.ai/) to create models for [Porcupine wake word engine](https://picovoice.ai/docs/quick-start/console-porcupine/) and [Rhino Speech-to-Intent engine](https://picovoice.ai/docs/quick-start/console-rhino/).
1. Select `Arm Cortex-M` as the platform when training the model.
1. Select `STM32` as the board type and provide the UUID of the chipset on the board.

The model is now being trained. You will be able to download it within a few hours.

## Import the Custom Models

1. Download your custom voice model(s) from [Picovoice Console](https://console.picovoice.ai/).
1. Decompress the zip file. The model file is either `.ppn` for Porcupine wake word or `.rhn` for Rhino Speech-to-Intent.
1. Use [binary_to_c_array.py](https://github.com/Picovoice/picovoice/tree/master/resources/scripts/binary_to_c_array.py) to convert your binary models to C array format  utilizing the following command:
`binary_to_c_array.py --binary_file_path INPUT_PATH --array_file_path OUTPUT_PATH`
1. Copy the content of `output_c_array.txt` and update the `keyword_array` and `context_array` values in [/stm32f769i-disco/Inc/pv_params.h](./stm32f769i-disco/Inc/pv_params.h).
 
