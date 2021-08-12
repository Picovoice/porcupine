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

- Rust 1.54+
- Runs on Linux (x86_64), macOS (x86_64), Windows (x86_64), Raspberry Pi, NVIDIA Jetson (Nano), and BeagleBone

## Installation
First you will need [Rust and Cargo](https://rustup.rs/) installed on your system.

To add the porcupine library into your app, add `pv_porcupine` to your apps `Cargo.toml` manifest:
```toml
[dependencies]
pv_porcupine = "*"
```

If you prefer to clone the repo and use it locally, first run `copy.sh`.
(**NOTE:** on Windows, Git Bash or another bash shell is required, or you will have to manually copy the libs into the project).
Then you can reference the local binding location:
```toml
[dependencies]
pv_porcupine = { path = "/path/to/rust/binding" }
```

## Usage

To create an instance of the engine you first create a PorcupineBuilder instance with the configuration parameters for the wake word engine and then make a call to `.init()`:

```rust
use porcupine::{BuiltinKeywords, PorcupineBuilder};

let porcupine: Porcupine = PorcupineBuilder::new_with_keywords(&[BuiltinKeywords::Porcupine]).init().expect("Unable to create Porcupine");
```
In the above example, we've initialzed the engine to detect the built-in wake word "Porcupine".
Built-in keywords are contained in the package with the `BuiltinKeywords` enum type.

Porcupine can detect multiple keywords concurrently:
```rust
let porcupine: Porcupine = PorcupineBuilder::new_with_keywords(&[BuiltinKeywords::Porcupine, BuiltinKeywords::Blueberry, BuiltinKeywords::Bumblebee])
    .init().expect("Unable to create Porcupine");
```

To detect custom keywords, use `PorupineBuilder`'s `new_with_keyword_paths` method to pass in `*.ppn` file paths instead:
```rust
let porcupine: Porcupine = PorcupineBuilder::new_with_keyword_paths(&["/absolute/path/to/keyword/one.ppn", "/absolute/path/to/keyword/two.ppn"])
    .init().expect("Unable to create Porcupine");
```

The language can be changed by passing in an appropriate `*.pv` file path into the `model_path` method:
```rust
let porcupine: Porcupine = PorcupineBuilder::new_with_keyword_paths(&["/absolute/path/to/keyword/one.ppn"])
    .model_path("/path/to/another/language_params.pv")
    .init().expect("Unable to create Porcupine");
```

The sensitivity of the engine can be tuned per keyword using the `sensitivities` method:
```rust
let porcupine: Porcupine = PorcupineBuilder::new_with_keywords(&[BuiltinKeywords::Porcupine, BuiltinKeywords::Bumblebee])
    .sensitivities(&[0.2f32, 0.42f32])
    .init().expect("Unable to create Porcupine");
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate.
It is a floating point number within `[0, 1]`.
A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

When initialized, the valid sample rate is given by `sample_rate()`.
Expected frame length (number of audio samples in an input array) is given by `frame_length()`.
The engine accepts 16-bit linearly-encoded PCM and operates on single-channel audio.

To feed audio into Porcupine, use the `process` function in your capture loop.
```rust
fn next_audio_frame() -> Vec<i16> {
    // get audio frame
}

loop {
    if let Ok(keyword_index) = porcupine.process(&next_audio_frame()) {
        if keyword_index >= 0 {
            // wake word detected!
        }   
    }
}
```

## Non-English Wake Words

In order to detect non-English wake words you need to use the corresponding model file.
The model files for all supported languages are available [here](/lib/common).

## Demos

Check out the Porcupine Rust demos [here](/demo/rust)

