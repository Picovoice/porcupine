# C Demos

## Compatibility

You need a C99-compatible compiler to build these demos.

## Requirements
- The demo requires [CMake](https://cmake.org/) version 3.4 or higher. 
- The microphone based demo requires [miniaudio](https://github.com/mackron/miniaudio) for accessing microphone audio data. 
- The following demo must be run at the [root](/../../) of the directory.
- **For Windows Only**: [MinGW](http://mingw-w64.org/doku.php) is required to build the demo.
   
# Microphone Demo

## Build

```console
cmake -S demo/c/. -B demo/c/build && cmake --build demo/c/build --target porcupine_demo_mic
```

## Run

### Usage

Running the executable without any commandline arguments prints the usage info to the console.

#### Linux, macOS, Raspberry Pi, BeagleBone, Jetson

```console
./demo/c/build/porcupine_demo_mic 
usage : ./demo/c/build/porcupine_demo_mic library_path model_path keyword_path sensitivity audio_device_intex
        ./demo/c/build/porcupine_demo_mic --show_audio_devices
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_mic.exe
usage : .\\demo\\c\\build\\porcupine_demo_mic.exe library_path model_path keyword_path sensitivity audio_device_intex
        .\\demo\\c\\build\\porcupine_demo_mic.exe --show_audio_devices
```

### Show Audio Devices

The following commands shows the available audio input devices to the console.

#### Linux, macOS, Raspberry Pi, BeagleBone, Jetson

```console
./demo/c/build/porcupine_demo_mic --show_audio_devices
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_mic.exe --show_audio_devices
```

### Wake Phrase

The following commands start up a microphone audio stream and listens for the wake phrase "porcupine".
Replace `${AUDIO_DEVICE_INDEX}` with the index of the audio device.

#### Linux

```console
./demo/c/build/porcupine_demo_mic lib/linux/x86_64/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/linux/porcupine_linux.ppn 0.5 ${AUDIO_DEVICE_INDEX}
```
#### macOS

```console
./demo/c/build/porcupine_demo_mic lib/mac/x86_64/libpv_porcupine.dylib lib/common/porcupine_params.pv \
resources/keyword_files/mac/porcupine_mac.ppn 0.5 ${AUDIO_DEVICE_INDEX}
```

#### Raspberry Pi

Replace `${PROCESSOR}` with one of the Raspberry Pi processors defined [here](../../lib/raspberry-pi) 
(e.g., for Raspberry Pi 4 this would be "cortex-a72") and run:

```console
./demo/c/build/porcupine_demo_mic lib/raspberry-pi/${PROCESSOR}/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/raspberry-pi/porcupine_raspberry-pi.ppn 0.5 ${AUDIO_DEVICE_INDEX}
```

#### BeagleBone

```console
./demo/c/build/porcupine_demo_mic lib/beaglebone/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/beaglebone/porcupine_beaglebone.ppn 0.5 ${AUDIO_DEVICE_INDEX}
```

#### Jetson

```console
./demo/c/build/porcupine_demo_mic lib/jetson/cortex-a57-aarch64/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/jetson/porcupine_jetson.ppn 0.5 ${AUDIO_DEVICE_INDEX}
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_mic.exe lib/windows/amd64/libpv_porcupine.dll lib/common/porcupine_params.pv resources/keyword_files/windows/porcupine_windows.ppn 0.5 ${AUDIO_DEVICE_INDEX}
```

# File Demo

## Build

```console
cmake -S demo/c/. -B demo/c/build && cmake --build demo/c/build --target porcupine_demo_file
```

## Run

### Usage

Running the executable without any commandline arguments prints the usage info to the console.

#### Linux, macOS, Raspberry Pi, BeagleBone, Jetson

```console
./demo/c/build/porcupine_demo_file 
usage : ./demo/c/build/porcupine_demo_file library_path model_path keyword_path sensitivity wav_path
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_file.exe
usage : .\\demo\\c\\build\\porcupine_demo_file.exe library_path model_path keyword_path sensitivity wav_path
```

### Wake Phrase

**Note that the demo expects a single-channel WAV file with a sampling rate of 16kHz and 16-bit linear PCM encoding. If you
provide a file with incorrect format the demo does not perform any format validation and simply outputs incorrect results.**

#### Linux

```console
./demo/c/build/porcupine_demo_file lib/linux/x86_64/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/linux/americano_linux.ppn 0.5 resources/audio_samples/multiple_keywords.wav 
```

#### macOS

```console
./demo/c/build/porcupine_demo_file lib/mac/x86_64/libpv_porcupine.dylib lib/common/porcupine_params.pv \
resources/keyword_files/mac/americano_mac.ppn 0.5 resources/audio_samples/multiple_keywords.wav 
```

#### Raspberry Pi

Replace `${PROCESSOR}` with one of the Raspberry Pi processors defined [here](../../lib/raspberry-pi)
(e.g., for Raspberry Pi 4 this would be "cortex-a72") and run:

```console
./demo/c/build/porcupine_demo_file lib/raspberry-pi/${PROCESSOR}/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/raspberry-pi/americano_raspberry-pi.ppn 0.5 resources/audio_samples/multiple_keywords.wav
```

#### BeagleBone

```console
./demo/c/build/porcupine_demo_file lib/beaglebone/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/beaglebone/americano_beaglebone.ppn 0.5 resources/audio_samples/multiple_keywords.wav
```

#### Jetson

```console
./demo/c/build/porcupine_demo_file lib/jetson/cortex-a57-aarch64/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/jetson/americano_jetson.ppn 0.5 resources/audio_samples/multiple_keywords.wav
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_file.exe lib/windows/amd64/libpv_porcupine.dll lib/common/porcupine_params.pv resources/keyword_files/windows/americano_windows.ppn 0.5 resources/audio_samples/multiple_keywords.wav
```
