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

- Go 1.16+
- Runs on Linux (x86_64), macOS (x86_64) and Windows (x86_64)

## Installation

```console
go get github.com/Picovoice/porcupine/binding/go
```

## Usage

To create an instance of the engine you first creat a Porcupine struct with the configuration parameters for the wake word engine and then make a call to `.Init()`.

```go
import . "github.com/Picovoice/porcupine/binding/go"

porcupine := Porcupine{BuiltInKeywords: []BuiltInKeyword{PICOVOICE}}
err := porcupine.Init()
if err != nil {
    // handle init fail
}
```
In the above example, we've initialzed the engine to detect the built-in wake word "Picovoice". Built-in keywords are constants in the package with the BuiltInKeyword type.

Porcupine can detect multiple keywords concurrently

```go
porcupine := Porcupine{BuiltInKeywords: []BuiltInKeyword{PICOVOICE, BUMBLEBEE}}
err := porcupine.Init()
```

To detect non-default keywords, use `KeywordPaths` parameter instead

```go
porcupine := Porcupine{KeywordPaths: []string{"/path/to/keyword.ppn"}}
err := porcupine.Init()
```

The sensitivity of the engine can be tuned per keyword using the `sensitivities` parameter

```go
porcupine := Porcupine{
    BuiltInKeywords: []BuiltInKeyword{PICOVOICE, BUMBLEBEE}
    Sensitivities: []float32{0.4, 0.9}}
err := porcupine.Init()
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating point number within `[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

When initialized, the valid sample rate is given by `SampleRate`. Expected frame length (number of audio samples in an input array) is given by `FrameLength`. The engine accepts 16-bit linearly-encoded PCM and operates on single-channel audio.

To feed audio into Porcupine, use the `Process` function in your capture loop. You must call `Init()` before calling `Process`. 
```go
func getNextFrameAudio() []int16{
    // get audio frame
}

for {
    keywordIndex, err := porcupine.Process(getNextFrameAudio())
    if keywordIndex >= 0 {
        // wake word detected!
    }
}
```

When done resources have to be released explicitly.

```go
porcupine.Delete()
```

Using a defer call to `Delete()` after `Init()` is also a good way to ensure cleanup.

## Non-English Wake Words

In order to detect non-English wake words you need to use the corresponding model file. The model files for all supported languages are available [here](/lib/common).

## Demos

Check out the Porcupine Go demos [here](/demo/go)

