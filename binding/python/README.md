# Porcupine Wake Word Engine

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient. It is perfect for IoT.
- cross-platform. Raspberry Pi, BeagleBone, Android, iOS, Linux (x86_64), macOS (x86_64), Windows (x86_64), and web
browsers are supported. Additionally, enterprise customers have access to ARM Cortex-M SDK.
- scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
- self-service. Developers can train custom wake word models using [Picovoice Console](https://picovoice.ai/console/).

## Compatibility

- Python 3
- Runs on Linux (x86_64), macOS (x86_64), Windows (x86_64), Raspberry Pi, and BeagleBone.

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
for accessing default keyword model files shipped with the package. The list of default keywords can be retrieved by

```python
import pvporcupine

print(pvporcupine.KEYWORDS)
```

Porcupine can detect multiple keywords concurrently

```python
import pvporcupine

handle = pvporcupine.create(keywords=['bumblebee', 'picovoice'])
```

To detect non-default keywords use `keyword_paths` input argument instead

```python
import pvporcupine

keyword_paths = ['/absolute/path/to/keyword/one', '/absolute/path/to/keyword/two', ...]

handle = pvporcupine.create(keyword_paths=keyword_paths)
```

The sensitivity of the engine can be tuned per keyword using the `sensitivities` input argument

```python
import pvporcupine

handle = pvporcupine.create(
        keywords=['grapefruit', 'porcupine'],
        sensitivities=[0.6, 0.35])
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

When initialized, the valid sample rate is given by `handle.sample_rate`. Expected frame length (number of audio samples
in an input array) is `handle.frame_length`. The engine accepts 16-bit linearly-encoded PCM and operates on
single-channel audio.

```python
def get_next_audio_frame():
    pass

while True:
    keyword_index = handle.process(get_next_audio_frame())
    if keyword_index >= 0:
        # detection event logic/callback
        pass
```

When done resources have to be released explicitly

```python
handle.delete()
```

## Demos

[pvporcupinedemo](https://pypi.org/project/pvporcupinedemo/) provides command-line utilities for processing real-time
audio (i.e. microphone) and files using Porcupine.