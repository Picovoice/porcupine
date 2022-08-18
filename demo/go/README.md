# Porcupine Go Demos

This Go module contains demos for processing real-time audio (i.e. microphone) and audio files using the Porcupine wake word engine.

## Requirements

- go 1.16+
- **Windows**: The demo requires `cgo`, which means that you need to install a gcc compiler like [Mingw](http://mingw-w64.org/) to build it properly. 

## Compatibility

- Linux (x86_64)
- macOS (x86_64, arm64)
- Windows (x86_64)
- Raspberry Pi:
  - Zero
  - 2
  - 3 (32 and 64 bit)
  - 4 (32 and 64 bit)
- NVIDIA Jetson Nano
- BeagleBone

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

NOTE: The working directory for the following go commands is:

```console
porcupine/demo/go
```

### File Demo

The file demo uses Porcupine to scan for keywords in a wave file. The demo is mainly useful for quantitative performance benchmarking against a corpus of audio data. 
Porcupine processes a 16kHz, single-channel audio stream. If a stereo file is provided it only processes the first (left) channel. 
The following processes a file looking for instances of the phrase "Picovoice":

```console
go run filedemo/porcupine_file_demo.go \
-input_audio_path "path/to/input.wav" \
-access_key "${ACCESS_KEY}" \
-keywords picovoice
```

The `keywords` argument refers to the built-in keyword files that are shipped with the package. The list of built-in keywords can be seen in the usage string:

```console
go run filedemo/porcupine_file_demo.go -h
```

To detect multiple phrases concurrently provide them as comma-separated values. If the wake word is more than a single word, surround the argument in quotation marks:

```console
go run filedemo/porcupine_file_demo.go \
-input_audio_path "path/to/input.wav" \
-access_key "${ACCESS_KEY}" \
-keywords "grasshopper,hey siri"
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://console.picovoice.ai/))
use `keyword_paths` argument:

```console
go run filedemo/porcupine_file_demo.go \
-input_audio_path "path/to/input.wav" \
-access_key "${ACCESS_KEY}" \
-keyword_paths "/path/to/keyword/one.ppn,/path/to/keyword/two.ppn"
```

The sensitivity of the engine can be tuned per keyword using the `sensitivities` input argument:

```console
go run filedemo/porcupine_file_demo.go \
-input_audio_path "path/to/input.wav" \
-access_key "${ACCESS_KEY}" \
-keywords grasshopper,porcupine \
-sensitivities 0.3,0.6
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

### Microphone Demo

This demo opens an audio stream from a microphone and detects utterances of a given wake word. The following opens the default
microphone and detects occurrences of "Picovoice":

```console
go run micdemo/porcupine_mic_demo.go \
-access_key "${ACCESS_KEY}" \
-keywords picovoice
```

The `keywords` argument refers to the built-in keyword files that are shipped with the package. The list of built-in keywords can be seen in the usage string:

```console
go run micdemo/porcupine_mic_demo.go -h
```

To detect multiple phrases concurrently provide them as comma-separated values. If the wake word is more than a single word, surround the argument in quotation marks:

```console
go run filedemo/porcupine_file_demo.go \
-access_key "${ACCESS_KEY}" \
-keywords "grasshopper,hey siri"
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://console.picovoice.ai/))
use `keyword_paths` argument:

```console
go run micdemo/porcupine_mic_demo.go \
-access_key "${ACCESS_KEY}" \
-keyword_paths "/path/to/keyword/one.ppn,/path/to/keyword/two.ppn"
```

It is possible that the default audio input device is not the one you wish to use. There are a couple
of debugging facilities baked into the demo application to solve this. First, type the following into the console:

```console
go run micdemo/porcupine_mic_demo.go -show_audio_devices
```

It provides information about various audio input devices on the box. Here is an example output:

```console
index: 0, device name: USB Audio Device
index: 1, device name: MacBook Air Microphone
``` 

You can use the device index to specify which microphone to use for the demo. For instance, if you want to use the USB Audio Device
in the above example, you can invoke the demo application as below:

```console
go run micdemo/porcupine_mic_demo.go \
-access_key "${ACCESS_KEY}" \
-keywords picovoice \
-audio_device_index 0
```

If the problem persists we suggest storing the recorded audio into a file for inspection. This can be achieved with:

```console
go run micdemo/porcupine_mic_demo.go \
-access_key "${ACCESS_KEY}" \
-keywords picovoice \
-audio_device_index 0 \
-output_path ./test.wav
```

If after listening to stored file there is no apparent problem detected please open an issue.
