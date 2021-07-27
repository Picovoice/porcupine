# Compatibility

You need a C99-compatible compiler to build these demos. The microphone based demo can run on
any device that supports `miniaudio`.

# Usage

## Microphone Demo

Compile by executing the following command from the root of the repository

```console
gcc -std=c99 -O3 -o demo/c/porcupine_demo_mic -I include/ demo/c/porcupine_demo_mic.c -ldl -lpthread -lm
```

Running the executable without any commandline arguments prints the usage info to the console as below

```console
$ ./demo/c/porcupine_demo_mic 
usage : ./demo/c/porcupine_demo_mic library_path model_path keyword_path sensitivity audio_device_intex
        ./demo/c/porcupine_demo_mic --show_audio_devices
```

To show the available audio input devices type following in the console
```console
$ ./demo/c/porcupine_demo_mic --show_audio_devices
```

The following starts an audio steaming from the microphone available on an Ubuntu 18.04 machine and listens for wake phrase
"porcupine" (leaving `audio_device_index` blank will resort to system default)

```console
./demo/c/porcupine_demo_mic lib/linux/x86_64/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/linux/porcupine_linux.ppn 0.5 (index)
```

The following achieves the same on a Raspberry Pi 4

```console
./demo/c/porcupine_demo_mic lib/raspberry-pi/cortex-a72/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/raspberry-pi/porcupine_raspberry-pi.ppn 0.5 (index)
```

## File Demo

**Note that the demo expect a single-channel WAV file with a sampling rate of 16000 and 16-bit linear PCM encoding. If you
provide a file with incorrect format the demo does not perform any format validation and simply outputs incorrect results.**

Compile by executing the following command from the root of the repository

```console
gcc -std=c99 -O3 -o demo/c/porcupine_demo_file -I include/ demo/c/porcupine_demo_file.c -ldl
```

Running the executable without any commandline arguments prints the usage info to the console as below

```console
$ ./demo/c/porcupine_demo_file
usage : ./demo/c/porcupine_demo_file library_path model_path keyword_path sensitivity wav_path
```

For example the following processes one of the WAV files under resources folder and scans it for keyword "Americano" on 
an Ubuntu 18.04

```console
./demo/c/porcupine_demo_file lib/linux/x86_64/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/linux/americano_linux.ppn 0.5 resources/audio_samples/multiple_keywords.wav 
```

The following achieves the same on a Raspberry Pi 4

```console
./demo/c/porcupine_demo_file lib/raspberry-pi/cortex-a72/libpv_porcupine.so lib/common/porcupine_params.pv \
resources/keyword_files/raspberry--pi.ppn 0.5 resources/audio_samples/multiple_keywords.wav
```
