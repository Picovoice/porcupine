# Porcupine Wake Word Engine Demos

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

This package contains demos and commandline utilities for processing real-time audio (i.e. microphone) and audio files
using Porcupine wake word engine.

## Porcupine

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient making it perfect for IoT.
- scalable. It can detect multiple always-listening voice commands with no added CPU/memory footprint.
- self-service. Developers can train custom wake phrases using [Picovoice Console](https://picovoice.ai/console/).

## Compatibility

- Python 3
- Runs on Linux (x86_64), Mac (x86_64), Windows (x86_64), Raspberry Pi (all variants), and BeagleBone.

## Installation

Microphone demo uses [PyAudio](https://people.csail.mit.edu/hubert/pyaudio/) for recording input audio. Consult the
installation guide at [PyAudio](https://people.csail.mit.edu/hubert/pyaudio/).

```bash
sudo pip3 install pvporcupinedemo
```

## Usage

### Microphone Demo

It opens an audio stream from a microphone and detects utterances of a given wake word. The following opens the default
microphone and detects occurrences of "Picovoice".

```bash
porcupine_demo_mic --keywords picovoice
```

`keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string

```bash
porcupine_demo_mic --help
```

To detect multiple phrases concurrently provide them as separate arguments

```bash
porcupine_demo_mic --keywords picovoice porcupine
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://picovoice.ai/console/))
use `keyword_paths` argument

```bash
porcupine_demo_mic --keyword_paths ${KEYWORD_PATH_ONE} ${KEYWORD_PATH_TWO}
```

It is possible that the default audio input device recognized by PyAudio is not the one being used. There are a couple
of debugging facilities baked into the demo application to solve this. First, type the following into the console

```bash
porcupine_demo_mic --show_audio_devices
```

It provides information about various audio input devices on the box. On a Linux box, this is the console output

```
'index': '0', 'name': 'HDA Intel PCH: ALC892 Analog (hw:0,0)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '2'
'index': '1', 'name': 'HDA Intel PCH: ALC892 Alt Analog (hw:0,2)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '2'
'index': '2', 'name': 'HDA NVidia: HDMI 0 (hw:1,3)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '3', 'name': 'HDA NVidia: HDMI 1 (hw:1,7)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '4', 'name': 'HDA NVidia: HDMI 2 (hw:1,8)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '5', 'name': 'HDA NVidia: HDMI 3 (hw:1,9)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '6', 'name': 'HDA NVidia: HDMI 0 (hw:2,3)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '7', 'name': 'HDA NVidia: HDMI 1 (hw:2,7)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '8', 'name': 'HDA NVidia: HDMI 2 (hw:2,8)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '9', 'name': 'HDA NVidia: HDMI 3 (hw:2,9)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '10', 'name': 'Logitech USB Headset: Audio (hw:3,0)', 'defaultSampleRate': '44100.0', 'maxInputChannels': '1'
'index': '11', 'name': 'sysdefault', 'defaultSampleRate': '48000.0', 'maxInputChannels': '128'
'index': '12', 'name': 'front', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '13', 'name': 'surround21', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '14', 'name': 'surround40', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '15', 'name': 'surround41', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '16', 'name': 'surround50', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '17', 'name': 'surround51', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '18', 'name': 'surround71', 'defaultSampleRate': '44100.0', 'maxInputChannels': '0'
'index': '19', 'name': 'pulse', 'defaultSampleRate': '44100.0', 'maxInputChannels': '32'
'index': '20', 'name': 'dmix', 'defaultSampleRate': '48000.0', 'maxInputChannels': '0'
'index': '21', 'name': 'default', 'defaultSampleRate': '44100.0', 'maxInputChannels': '32'
``` 

It can be seen that the last device (index 21) is considered default. But on this machine, a headset is being used as 
the input device which has an index of 10. After finding the correct index the demo application can be invoked as below

```bash
porcupine_demo_mic --keywords picovoice --audio_device_index 10
```

If the problem persists we suggest storing the recorded audio into a file for inspection. This can be achieved by

```bash
porcupine_demo_mic --keywords picovoice --audio_device_index 10 --output_path ~/test.wav
```

If after listening to stored file there is no apparent problem detected please open an issue.

### File Demo

It allows testing Porcupine on a corpus of audio files. The demo is mainly useful for quantitative performance
benchmarking. It accepts 16kHz audio files. Porcupine processes a single-channel audio stream if a stereo file is
provided it only processes the first (left) channel. The following processes a file looking for instances of the phrase
"Picovoice"

```bash
porcupine_demo_file --input_audio_path ${AUDIO_PATH} --keywords picovoice
```

`keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string

```bash
porcupine_demo_file --help
```

To detect multiple phrases concurrently provide them as separate arguments

```bash
porcupine_demo_file --input_audio_path ${AUDIO_PATH} --keywords grasshopper porcupine
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://picovoice.ai/console/))
use `keyword_paths` argument

```bash
porcupine_demo_file --input_audio_path ${AUDIO_PATH} \
--keyword_paths ${KEYWORD_PATH_ONE} ${KEYWORD_PATH_TWO}
```

The sensitivity of the engine can be tuned per keyword using the `sensitivities` input argument

```bash
porcupine_demo_file --input_audio_path ${AUDIO_PATH} \
--keywords grasshopper porcupine --sensitivities 0.3 0.6
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.
