# Compatibility

You need a C99-compatible compiler to build these demos. The microphone based demo can only run on Linux-based systems
such as Ubuntu, Raspberry Pi, and Beagle Bone as it depends on ALSA.

# Usage

## Microphone-Based

Compile by executing the following command from the root of the repository

```bash
gcc -O3 -o dem/c/porcupine_demo_mic -I include/ demo/c/porcupine_demo_mic.c -ldl -lasound
```

## File-Based

Compile by executing the following command from the root of the repository

```bash
gcc -O3 -o dem/c/porcupine_demo_file-I include/ demo/c/porcupine_demo_file.c -ldl -lasound
```