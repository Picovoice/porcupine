# Porcupine Python SDK

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient making it perfect for IoT.
- cross-platform. It is implemented in fixed-point ANSI C. Raspberry Pi (all variants), Beagle Bone, Android, iOS,
  watchOS, Linux (x86_64), Mac (x86_64), Windows (x86_64), and web browsers are supported. Furthermore, Support for
  various ARM Cortex-A microprocessors and ARM Cortex-M microcontrollers is available for enterprise customers.
- scalable. It can detect multiple always-listening voice commands with no added CPU/memory footprint.
- self-service. Developers can train custom wake phrases using [Picovoice Console](https://picovoice.ai/console/).

## Compatibility

- Python 3
- Runs on Linux (x86_64), Mac (x86_64), Windows (x86_64), Raspberry Pi (all variants), and Beagle Bone.

## Installation

```bash
pip3 install pvporcupine
```

## Usage

Create an instance of the engine

```python
import pvporcupine

handle = pvporcupine.create(keywords=['picovoice'])
```

`handle` is an instance of Porcupine that detects utterances of "Picovoice". `keywords` input argument is a shorthand
for accessing default keyword files shipped with the package. The list of default keywords can be retrieved by

```python
import pvporcupine

print(pvporcupine.KEYWORDS)
```

Porcupine can detect multiple keywords concurrently

```python
import pvporcupine

handle = pvporcupine.create(keywords=['picovoice', 'bumblebee'])
```

If you wish to use a non-default keyword file, you need to use `keyword_file_paths` input argument instead

```python
import pvporcupine

handle = pvporcupine.create(keyword_file_paths=['absolute/path/to/keyword/one', 'absolute/path/to/keyword/two', ...])
```

The sensitivity of the engine can be tuned using the `sensitivities` input argument

```python
import pvporcupine

handle = pvporcupine.create(keywords=['grapefruit', 'porcupine'], sensitivities=[0.6, 0.35])
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
`[0, 1]`. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

When initialized, valid sample rate can be obtained using `handle.sample_rate`. Expected frame length (number of audio
samples in an input array) is `handle.frame_length`. The engine accepts 16-bit linearly-encoded PCM. The object can be
used to monitor incoming audio as below.

```python
def get_next_audio_frame():
    pass

while True:
    keyword_index = handle.process(get_next_audio_frame())
    if keyword_index >= 0:
        # detection event logic/callback
        pass
```

Finally, when done be sure to explicitly release the resources

```python
handle.delete()
```

## Demos

[pvporcupinedemo](https://pypi.org/project/pvporcupinedemo/) provides command-line utilities for processing real-time
audio (i.e. microphone) and files using Porcupine engine. The source code for these utilities is available on
Porcupine's GitHub repository:

- [File Demo](/demo/python/porcupine_demo_file.py)
- [Microphone Demo](/demo/python/porcupine_demo_mic.py)
