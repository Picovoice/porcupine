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
- self-service. Developers can train custom wake phrases using [Picovoice Console](https://console.picovoice.ai/).

## Compatibility

- Python 3.5+
- Runs on Linux (x86_64), Mac (x86_64 and arm64), Windows (x86_64), Raspberry Pi (all variants), NVIDIA Jetson (Nano), and BeagleBone.

## Installation

```console
sudo pip3 install pvporcupinedemo
```

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

### Microphone Demo

It opens an audio stream from a microphone and detects utterances of a given wake word. The following opens the default
microphone and detects occurrences of "Picovoice".

```console
porcupine_demo_mic --access_key ${ACCESS_KEY} --keywords picovoice
```

`keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string

```console
porcupine_demo_mic --help
```

To detect multiple phrases concurrently provide them as separate arguments

```console
porcupine_demo_mic --access_key ${ACCESS_KEY} --keywords picovoice porcupine
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://console.picovoice.ai/))
use `keyword_paths` argument

```console
porcupine_demo_mic --access_key ${ACCESS_KEY} --keyword_paths ${KEYWORD_PATH_ONE} ${KEYWORD_PATH_TWO}
```

To detect non-English keywords provide the respective model path:

```console
porcupine_demo_mic --access_key ${ACCESS_KEY} --model_path ${NON_ENGLISH_MODEL_PATH} --keyword_paths ${NON_ENGLISH_KEYWORD_PATH}
```

The model files for all supported languages are available 
[here](https://github.com/Picovoice/porcupine/tree/master/lib/common) on Porcupine's GitHub repository.

It is possible that the default audio input device recognized by the demo is not the one being used. There are a couple of
debugging facilities baked into the demo application to solve this. First, type the following into the console:

```console
porcupine_demo_mic --show_audio_devices
```

It provides information about various audio input devices on the box. On a Linux box, this is the console output

```
index: 0, device name: USB Audio Device
index: 1, device name: MacBook Air Microphone
``` 

You can use the device index to specify which microphone to use for the demo. For instance, if you want to use the USB Audio Device 
in the above example, you can invoke the demo application as below:

```console
porcupine_demo_mic --access_key ${ACCESS_KEY} --keywords picovoice --audio_device_index 0
```

If the problem persists we suggest storing the recorded audio into a file for inspection. This can be achieved by

```console
porcupine_demo_mic --access_key ${ACCESS_KEY} --keywords picovoice --audio_device_index 0 --output_path ~/test.wav
```

If after listening to stored file there is no apparent problem detected please open an issue.

### File Demo

It allows testing Porcupine on a corpus of audio files. The demo is mainly useful for quantitative performance
benchmarking. It accepts 16kHz audio files. Porcupine processes a single-channel audio stream if a stereo file is
provided it only processes the first (left) channel. The following processes a file looking for instances of the phrase
"Picovoice"

```console
porcupine_demo_file --access_key ${ACCESS_KEY} --wav_path ${AUDIO_PATH} --keywords picovoice
```

`keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string

```console
porcupine_demo_file --help
```

To detect multiple phrases concurrently provide them as separate arguments

```console
porcupine_demo_file --access_key ${ACCESS_KEY} --wav_path ${AUDIO_PATH} --keywords grasshopper porcupine
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://console.picovoice.ai/))
use `keyword_paths` argument

```console
porcupine_demo_file --access_key ${ACCESS_KEY} \
--wav_path ${AUDIO_PATH} \
--keyword_paths ${KEYWORD_PATH_ONE} ${KEYWORD_PATH_TWO}
```

To detect non-English keywords provide the respective model path:

```console
porcupine_demo_mic --access_key ${ACCESS_KEY} \
--wav_path ${AUDIO_PATH} \
--model_path ${NON_ENGLISH_MODEL_PATH} \
--keyword_paths ${NON_ENGLISH_KEYWORD_PATH} 
```

The model files for all supported languages are available 
[here](https://github.com/Picovoice/porcupine/tree/master/lib/common) on Porcupine's GitHub repository.

The sensitivity of the engine can be tuned per keyword using the `sensitivities` input argument

```console
porcupine_demo_file --access_key ${ACCESS_KEY} \
--wav_path ${AUDIO_PATH} \
--keywords grasshopper porcupine --sensitivities 0.3 0.6
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.
