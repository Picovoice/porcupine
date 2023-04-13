# Porcupine Binding for .NET

## Porcupine Wake Word Engine

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications.

Porcupine is:

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient. It is perfect for IoT.
- scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
- self-service. Developers can train custom wake word models using [Picovoice Console](https://console.picovoice.ai/).

## Requirements

- .NET 6.0

## Compatibility

Platform compatible with .NET Framework 4.6.1+:

- Windows (x86_64)

Platforms compatible with .NET Core 2.0+:

- Linux (x86_64)
- macOS (x86_64)
- Windows (x86_64)

Platforms compatible with .NET Core 3.0+:

- Raspberry Pi:
  - 2
  - 3 (32 and 64 bit)
  - 4 (32 and 64 bit)
- NVIDIA Jetson Nano
- BeagleBone

Platform compatible with .NET 6.0+:

- macOS (arm64)

## Installation

You can install the latest version of Porcupine by getting the latest [Porcupine Nuget package](https://www.nuget.org/packages/Porcupine/)
in Visual Studio or using the .NET CLI.

```console
dotnet add package Porcupine
```

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

Create an instance of the engine:

```csharp
using Pv;

const string accessKey = "${ACCESS_KEY}";
var keyword = new List<BuiltInKeyword> { BuiltInKeyword.PICOVOICE };

Porcupine handle = Porcupine.FromBuiltInKeywords(accessKey, keyword);
```

`handle` is an instance of Porcupine that detects utterances of "Picovoice". Using the `FromBuiltInKeywords` constructor allows you to initialize the Porcupine engine to detect any of the free, built-in keywords that come with the library. These built-ins are represented by the `BuiltInKeyword` enum.

Porcupine can detect multiple keywords concurrently:

```csharp
const string accessKey = "${ACCESS_KEY}";
var keywords = new List<BuiltInKeyword> {
        BuiltInKeyword.BUMBLEBEE,
        BuiltInKeyword.PICOVOICE
    };

Porcupine handle = Porcupine.FromBuiltInKeywords(accessKey, keywords);
```

To detect custom keywords, use the `FromKeywordPaths` constructor instead:

```csharp
const string accessKey = "${ACCESS_KEY}";
var keywordPaths = new List<string> {
    "/absolute/path/to/keyword/one",
    "/absolute/path/to/keyword/two",
    ... }

Porcupine handle = Porcupine.FromKeywordPaths(accessKey, keywordPaths);
```

In addition to custom keywords, you can override the default Porcupine english model file and/or keyword sensitivities.

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating-point number within [0, 1]. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

The model file contains the parameters for the wake word engine. To change the language that Porcupine understands, you'll pass in a different model file.

```csharp
const string accessKey = "${ACCESS_KEY}";
var keywords = new List<BuiltInKeyword> {
        BuiltInKeyword.GRAPEFRUIT,
        BuiltInKeyword.PORCUPINE
    };
string modelPath = "/path/to/model.pv"
var sensitivities = new List<float>{ 0.6f, 0.35f };

Porcupine handle = Porcupine.FromBuiltInKeywords(
    accessKey,
    keywords,
    modelPath: modelPath,
    sensitivities: sensitivities);
```

When initialized, the valid sample rate is given by `handle.SampleRate`. Expected frame length (number of audio samples
in an input array) is `handle.FrameLength`. The engine accepts 16-bit linearly-encoded PCM and operates on
single-channel audio.

```csharp
short[] GetNextAudioFrame()
{
    // .. get audioFrame
    return audioFrame;
}

while(true)
{
    var keywordIndex = handle.Process(GetNextAudioFrame());
    if(keywordIndex >= 0)
    {
	    // .. detection event logic/callback
    }
}
```

Porcupine will have its resources freed by the garbage collector, but to have resources freed  immediately after use,
wrap it in a using statement:

```csharp
using(Porcupine handle = Porcupine.FromBuiltInKeywords(
    accessKey,
    new List<BuiltInKeyword> { BuiltInKeyword.PICOVOICE }))
{
    // .. Porcupine usage here
}
```

## Non-English Wake Words

In order to detect non-English wake words you need to use the corresponding model file. The model files for all supported languages are available [here](https://github.com/Picovoice/porcupine/tree/master/lib/common).

## Demos

The [Porcupine dotnet demo project](https://github.com/Picovoice/porcupine/tree/master/demo/dotnet) is a .NET Core command line application that allows for
processing real-time audio (i.e. microphone) and files using Porcupine.
