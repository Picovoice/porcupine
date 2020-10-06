# Porcupine Wake Word Engine Demos

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

This package contains .NET Core Console demos for processing real-time audio (i.e. microphone) and audio files
using Porcupine wake word engine.

## Porcupine

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient making it perfect for IoT.
- scalable. It can detect multiple always-listening voice commands with no added CPU/memory footprint.
- self-service. Developers can train custom wake phrases using [Picovoice Console](https://picovoice.ai/console/).

## Compatibility

- .NET Core 3.0+
- Runs on Linux (x86_64), Mac (x86_64) and Windows (x86_64)

## Installation

Both demos use [Microsoft's .NET Core framework](https://dotnet.microsoft.com/download). Once you have .NET Core installed in your environment, you can build the demos.

```bash
dotnet build -c MicDemo.Release
dotnet build -c FileDemo.Release
```

## Usage

### File Demo

It allows testing Porcupine on a corpus of audio files. The demo is mainly useful for quantitative performance
benchmarking. It accepts 16kHz audio files. Porcupine processes a single-channel audio stream if a stereo file is
provided it only processes the first (left) channel. The following processes a file looking for instances of the phrase
"Picovoice"

```bash
dotnet run -c FileDemo.Release -- --input_audio_path ${AUDIO_PATH} --keywords picovoice
```

`keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string

```bash
dotnet run -c FileDemo.Release -- --help
```

To detect multiple phrases concurrently provide them as separate arguments

```bash
dotnet run -c FileDemo.Release -- --input_audio_path ${AUDIO_PATH} --keywords grasshopper porcupine
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://picovoice.ai/console/))
use `keyword_paths` argument

```bash
dotnet run -c FileDemo.Release -- --input_audio_path ${AUDIO_PATH} \
--keyword_paths ${KEYWORD_PATH_ONE} ${KEYWORD_PATH_TWO}
```

The sensitivity of the engine can be tuned per keyword using the `sensitivities` input argument

```bash
dotnet run -c FileDemo.Release -- --input_audio_path ${AUDIO_PATH} \
--keywords grasshopper porcupine --sensitivities 0.3 0.6
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

### Microphone Demo

It opens an audio stream from a microphone and detects utterances of a given wake word. The following opens the default
microphone and detects occurrences of "Picovoice".

```bash
dotnet run -c MicDemo.Release -- --keywords picovoice
```

`keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string

```bash
dotnet run -c MicDemo.Release -- --help
```

To detect multiple phrases concurrently provide them as separate arguments

```bash
dotnet run -c MicDemo.Release -- --keywords picovoice porcupine
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://picovoice.ai/console/))
use `keyword_paths` argument

```bash
dotnet run -c MicDemo.Release -- --keyword_paths ${KEYWORD_PATH_ONE} ${KEYWORD_PATH_TWO}
```

It is possible that the default audio input device is not the one you wish to use. There are a couple
of debugging facilities baked into the demo application to solve this. First, type the following into the console

```bash
dotnet run -c MicDemo.Release -- --show_audio_devices
```

It provides information about various audio input devices on the box. On a Windows PC, this is the output:

```
Available input devices:

	Device 0: Microphone Array (Realtek(R) Au, 2 channels
	Device 1: Microphone Headset USB, 1 channel	
``` 

You can use the device index to specify which microphone to use for the demo. For instance, if you want to use the Headset 
microphone in the above example, you can invoke the demo application as below

```bash
dotnet run -c MicDemo.Release -- --keywords picovoice --audio_device_index 1
```

If the problem persists we suggest storing the recorded audio into a file for inspection. This can be achieved with

```bash
dotnet run -c MicDemo.Release -- --keywords picovoice --audio_device_index 1 --output_path ./test.wav
```

If after listening to stored file there is no apparent problem detected please open an issue.