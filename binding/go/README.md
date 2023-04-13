# Porcupine Binding for Go

## Porcupine Wake Word Engine

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient. It is perfect for IoT.
- cross-platform:
  - Arm Cortex-M, STM32, Arduino, and i.MX RT
  - Raspberry Pi, NVIDIA Jetson Nano, and BeagleBone
  - Android and iOS
  - Chrome, Safari, Firefox, and Edge
  - Linux (x86_64), macOS (x86_64, arm64), and Windows (x86_64)
- scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
- self-service. Developers can train custom wake word models using [Picovoice Console](https://console.picovoice.ai/).

## Compatibility

- Go 1.16+
- Runs on Linux (x86_64), macOS (x86_64, arm64), Windows (x86_64), Raspberry Pi, NVIDIA Jetson (Nano), and BeagleBone.

## Installation

```console
go get github.com/Picovoice/porcupine/binding/go/v2
```

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

To create an instance of the engine you first create a Porcupine struct with the configuration parameters for the wake word engine and then make a call to `.Init()`:

```go
import . "github.com/Picovoice/porcupine/binding/go/v2"

porcupine := Porcupine{
    AccessKey: "${ACCESS_KEY}", // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
    BuiltInKeywords: []BuiltInKeyword{PICOVOICE}}
err := porcupine.Init()
if err != nil {
    // handle init fail
}
```
In the above example, we've initialized the engine to detect the built-in wake word "Picovoice". Built-in keywords are constants in the package with the BuiltInKeyword type.

Porcupine can detect multiple keywords concurrently:
```go
porcupine := Porcupine{
    AccessKey: "${ACCESS_KEY}", // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
    BuiltInKeywords: []BuiltInKeyword{PICOVOICE, BUMBLEBEE}}
err := porcupine.Init()
```

To detect non-default keywords, use `KeywordPaths` parameter instead:
```go
porcupine := Porcupine{
    AccessKey: "${ACCESS_KEY}", // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
    KeywordPaths: []string{"/path/to/keyword.ppn"}}
err := porcupine.Init()
```

In addition to custom keywords, you can override the default Porcupine english model file and/or keyword sensitivities.

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating-point number within `[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

The model file contains the parameters for the wake word engine. To change the language that Porcupine understands, you'll pass in a different model file.

```go
porcupine := Porcupine{
    AccessKey: "${ACCESS_KEY}", // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
    BuiltInKeywords: []BuiltInKeyword{PICOVOICE, BUMBLEBEE},
    Sensitivities: []float32{0.4, 0.9},
    ModelPath: "/path/to/model.pv"}
err := porcupine.Init()
```

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

Using a `defer` call to `Delete()` after `Init()` is also a good way to ensure cleanup.

## Non-English Wake Words

In order to detect non-English wake words you need to use the corresponding model file. The model files for all supported languages are available [here](https://github.com/Picovoice/porcupine/tree/master/lib/common).

## Demos

Check out the Porcupine Go demos [here](https://github.com/Picovoice/porcupine/tree/master/demo/go)
