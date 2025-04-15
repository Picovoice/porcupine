# C Demos

## Compatibility

You need a C99-compatible compiler to build these demos.

## Requirements
- The demo requires [CMake](https://cmake.org/) version 3.13 or higher.
- **For Windows Only**: [MinGW](http://mingw-w64.org/) is required to build the demo.

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Build Linux/MacOS

```console
cmake -S demo/c/. -B demo/c/build -DPV_RECORDER_PLATFORM={PV_RECORDER_PLATFORM}
cmake --build demo/c/build --target porcupine_demo_mic
```

The {PV_RECORDER_PLATFORM} variable will set the compilation flags for the given platform. Exclude this variable to get a list of possible values.

## Build Windows

```console
cmake -S demo/c/. -B demo/c/build -DPV_RECORDER_PLATFORM={PV_RECORDER_PLATFORM} -G "MinGW Makefiles"
cmake --build demo/c/build --target porcupine_demo_mic
```

The {PV_RECORDER_PLATFORM} variable will set the compilation flags for the given platform. Exclude this variable to get a list of possible values.

## Run

### Usage

Running the executable without any commandline arguments prints the usage info to the console.

#### Linux, macOS, Raspberry Pi

```console
./demo/c/build/porcupine_demo_mic 
Usage : ./demo/c/build/porcupine_demo_mic -l LIBRARY_PATH -m MODEL_PATH -k KEYWORD_PATH -t SENSITIVITY -a ACCESS_KEY -d AUDIO_DEVICE_INDEX
        ./demo/c/build/porcupine_demo_mic [-s, --show_audio_devices]
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_mic.exe
Usage : .\\demo\\c\\build\\porcupine_demo_mic.exe -l LIBRARY_PATH -m MODEL_PATH -k KEYWORD_PATH -t SENSITIVITY -a ACCESS_KEY -d AUDIO_DEVICE_INDEX
        .\\demo\\c\\build\\porcupine_demo_mic.exe [-s, --show_audio_devices]
```

### Show Audio Devices

The following commands shows the available audio input devices to the console.

#### Linux, macOS, Raspberry Pi

```console
./demo/c/build/porcupine_demo_mic --show_audio_devices
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_mic.exe --show_audio_devices
```

### Wake Phrase

The following commands start up a microphone audio stream and listens for the wake phrase "porcupine".
Replace `${AUDIO_DEVICE_INDEX}` with the index of the audio device and `${ACCESS_KEY}` with your `AccessKey`.

#### Linux

```console
./demo/c/build/porcupine_demo_mic \
-l lib/linux/x86_64/libpv_porcupine.so -m lib/common/porcupine_params.pv \
-k resources/keyword_files/linux/porcupine_linux.ppn -t 0.5 \
-a ${ACCESS_KEY} -d ${AUDIO_DEVICE_INDEX}
```
#### macOS

```console
# Macos with Intel Chip
./demo/c/build/porcupine_demo_mic \
-l lib/mac/x86_64/libpv_porcupine.dylib -m lib/common/porcupine_params.pv \
-k resources/keyword_files/mac/porcupine_mac.ppn -t 0.5 \
-a ${ACCESS_KEY} -d ${AUDIO_DEVICE_INDEX}

# Macos with Apple Chip
./demo/c/build/porcupine_demo_mic \
-l lib/mac/arm64/libpv_porcupine.dylib -m lib/common/porcupine_params.pv \
-k resources/keyword_files/mac/porcupine_mac.ppn -t 0.5 \
-a ${ACCESS_KEY} -d ${AUDIO_DEVICE_INDEX}
```

#### Raspberry Pi

Replace `${PROCESSOR}` with one of the Raspberry Pi processors defined [here](../../lib/raspberry-pi) 
(e.g., for Raspberry Pi 4 this would be "cortex-a72") and run:

```console
./demo/c/build/porcupine_demo_mic \
-l lib/raspberry-pi/${PROCESSOR}/libpv_porcupine.so -m lib/common/porcupine_params.pv \
-k resources/keyword_files/raspberry-pi/porcupine_raspberry-pi.ppn -t 0.5 \
-a ${ACCESS_KEY} -d ${AUDIO_DEVICE_INDEX}
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_mic.exe -l lib/windows/amd64/libpv_porcupine.dll -m lib/common/porcupine_params.pv -k resources/keyword_files/windows/porcupine_windows.ppn -t 0.5 -a ${ACCESS_KEY} -d ${AUDIO_DEVICE_INDEX}
```

# File Demo

## Build

```console
cmake -S demo/c/. -B demo/c/build -DPV_RECORDER_PLATFORM={PV_RECORDER_PLATFORM}
cmake --build demo/c/build --target porcupine_demo_file
```

The {PV_RECORDER_PLATFORM} variable will set the compilation flags for the given platform. Exclude this variable to get a list of possible values.

## Run

### Usage

Running the executable without any commandline arguments prints the usage info to the console.

#### Linux, macOS, Raspberry Pi

```console
./demo/c/build/porcupine_demo_file 
Usage : ./demo/c/build/porcupine_demo_file -l LIBRARY_PATH -m MODEL_PATH -k KEYWORD_PATH -t SENSITIVITY -a ACCESS_KEY -w WAV_PATH
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_file.exe
Usage : .\\demo\\c\\build\\porcupine_demo_file.exe -l LIBRARY_PATH -m MODEL_PATH -k KEYWORD_PATH -t SENSITIVITY -a ACCESS_KEY -w WAV_PATH
```

### Wake Phrase

**Note that the demo expects a single-channel WAV file with a sampling rate of 16kHz and 16-bit linear PCM encoding.**

Replace `${ACCESS_KEY}` with your `AccessKey`.

#### Linux

```console
./demo/c/build/porcupine_demo_file -l lib/linux/x86_64/libpv_porcupine.so -m lib/common/porcupine_params.pv \
-k resources/keyword_files/linux/americano_linux.ppn -t 0.5 -w resources/audio_samples/multiple_keywords.wav \
-a ${ACCESS_KEY}
```

#### macOS

```console
# Macos with Intel Chip
./demo/c/build/porcupine_demo_file -l lib/mac/x86_64/libpv_porcupine.dylib -m lib/common/porcupine_params.pv \
-k resources/keyword_files/mac/americano_mac.ppn -t 0.5 -w resources/audio_samples/multiple_keywords.wav \
-a ${ACCESS_KEY}

# Macos with Apple Chip
./demo/c/build/porcupine_demo_file -l lib/mac/arm64/libpv_porcupine.dylib -m lib/common/porcupine_params.pv \
-k resources/keyword_files/mac/americano_mac.ppn -t 0.5 -w resources/audio_samples/multiple_keywords.wav \
-a ${ACCESS_KEY}
```

#### Raspberry Pi

Replace `${PROCESSOR}` with one of the Raspberry Pi processors defined [here](../../lib/raspberry-pi)
(e.g., for Raspberry Pi 4 this would be "cortex-a72") and run:

```console
./demo/c/build/porcupine_demo_file -l lib/raspberry-pi/${PROCESSOR}/libpv_porcupine.so -m lib/common/porcupine_params.pv \
-k resources/keyword_files/raspberry-pi/americano_raspberry-pi.ppn -t 0.5 -w resources/audio_samples/multiple_keywords.wav \
-a ${ACCESS_KEY}
```

#### Windows

```console
.\\demo\\c\\build\\porcupine_demo_file.exe -l lib/windows/amd64/libpv_porcupine.dll -m lib/common/porcupine_params.pv -k resources/keyword_files/windows/americano_windows.ppn -t 0.5 -w resources/audio_samples/multiple_keywords.wav -a ${ACCESS_KEY}
```
