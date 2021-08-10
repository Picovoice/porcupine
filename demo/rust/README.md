# Porcupine Rust Demos

This Rust module contains demos for processing real-time audio (i.e. microphone) and audio files using the Porcupine wake word engine.

## Installation

MicDemo uses [miniaudio-rs](https://github.com/ExPixel/miniaudio-rs) for cross-platform audio capture. It uses `bindgen` and therefore requires `clang` to be installed and on the path. Use the [`Bindgen` docs](https://rust-lang.github.io/rust-bindgen/requirements.html) for instructions on how to install `clang` for various Operating Systems and distros. 

## Usage

NOTE: The working directory for the following `Cargo` commands is:

```console
porcupine/demo/rust/filedemo  # File Demo
porcupine/demo/rust/micdemo  # Mic Demo
```

### File Demo

The file demo uses Porcupine to scan for keywords in a `.wav` file. The demo is mainly useful for quantitative performance benchmarking against a corpus of audio data. s
Porcupine processes a 16kHz, single-channel audio stream. If a stereo file is provided it only processes the first (left) channel. 
The following processes a file looking for instances of the phrase "Picovoice":

```console
cargo run --release -- --input_audio_path "path/to/input.wav" --keywords picovoice
```

`keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string:

```console
cargo run --release -- --help
```

To detect multiple phrases concurrently provide them as comma-seperated values. If the wake word is more than a single word, surround the argument in quotation marks:

```console
cargo run --release -- --input_audio_path "path/to/input.wav" --keywords picovoice,grasshopper,"hey siri"
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://picovoice.ai/console/))
use `keyword_paths` argument:

```console
cargo run --release -- --input_audio_path "path/to/input.wav" \
--keyword_paths "/path/to/keyword/one.ppn","/path/to/keyword/two.ppn"
```

The sensitivity of the engine can be tuned per keyword using the `sensitivities` input argument:

```console
cargo run --release -- --input_audio_path "path/to/input.wav" \
--keywords grasshopper,porcupine --sensitivities 0.3,0.6
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

### Microphone Demo

This demo opens an audio stream from a microphone and detects utterances of a given wake word. The following opens the default
microphone and detects occurrences of "Picovoice":

```console
cargo run --release -- --keywords picovoice
```

`keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string:

```console
cargo run --release -- --help
```

To detect multiple phrases concurrently provide them as comma-seperated values. If the wake word is more than a single word, surround the argument in quotation marks:

```console
cargo run --release -- --keywords picovoice,grasshopper,"hey siri"
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://picovoice.ai/console/))
use `keyword_paths` argument:

```console
cargo run --release -- --keyword_paths "/path/to/keyword/one.ppn","/path/to/keyword/two.ppn"
```

It is possible that the default audio input device is not the one you wish to use. There are a couple
of debugging facilities baked into the demo application to solve this. First, type the following into the console:

```console
cargo run --release -- --show_audio_devices
```

It provides information about various audio input devices on the box. On a is an example output from a Windows machine:

```console
Capture Devices
    0: Microphone Array (Realtek(R) Audio)
    1: Microphone (USB Audio Device)
``` 

You can use the device index to specify which microphone to use for the demo. For instance, if you want to use the USB 
microphone in the above example, you can invoke the demo application as below:

```console
cargo run --release -- --keywords picovoice --audio_device_index 1
```

Exact system setups don't always play well with certain audio backends. If this is the case you can override the default with a specific backend:

```console
cargo run -- --release --keywords picovoice --audio_device_index 1 --audio_backend Alsa
```

If the problem persists we suggest storing the recorded audio into a file for inspection. This can be achieved with:

```console
cargo run --release -- --keywords picovoice --output_path ./test.wav
```

If after listening to stored file there is no apparent problem detected please open an issue.
