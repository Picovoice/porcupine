# Porcupine Wake Word Engine

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. 

Porcupine is:

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient. It is perfect for IoT.
- scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
- self-service. Developers can train custom wake word models using [Picovoice Console](https://picovoice.ai/console/).

## Requirements

- .NET Core 3.1

## Compatibility

Platform compatible with .NET Framework 4.6.1+:

- Windows (x86_64)

Platforms compatible with .NET Core 2.0+:

- Linux (x86_64)
- macOS (x86_64)
- Windows (x86_64)

Platforms compatible with .NET Core 3.1+:

- Raspberry Pi:
  - 2
  - 3 (32 and 64 bit)
  - 4 (32 and 64 bit)
- NVIDIA Jetson Nano
- BeagleBone

## Installation

You can install the latest version of Porcupine by getting the latest [Porcupine Nuget package](https://www.nuget.org/packages/Porcupine/)
in Visual Studio or using the .NET CLI.

```console
dotnet add package Porcupine
```

## Usage

Create an instance of the engine

```csharp
using Pv;

Porcupine handle = Porcupine.Create(keywords: new List<string> { "picovoice" });
```

`handle` is an instance of Porcupine that detects utterances of "Picovoice". The `keywords` input argument is a shorthand
for accessing default keyword model files shipped with the package. The list of default keywords can be retrieved by

```csharp
using Pv;

foreach (string keyword in Porcupine.KEYWORDS)
{
    Console.WriteLine(keyword);
}
```

Porcupine can detect multiple keywords concurrently

```csharp
using Pv;

Porcupine handle = Porcupine.Create(keywords: new List<string>{ "bumblebee", "picovoice" });
```

To detect non-default keywords use the `keywordPaths` input argument instead

```csharp
using Pv;

var keywordPaths = new List<string>{ "/absolute/path/to/keyword/one", "/absolute/path/to/keyword/two", ...}

Porcupine handle = Porcupine.Create(keywordPaths: keywordPaths);
```

The sensitivity of the engine can be tuned per-keyword using the `sensitivities` input argument

```csharp
using Pv;

Porcupine handle = Porcupine.Create(keywords: new List<string>{ "grapefruit", "porcupine" },  
                                    sensitivities: new List<float>{ 0.6f, 0.35f });
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

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
using(Porcupine handle = Porcupine.Create(keywords: new List<string> { "picovoice" }))
{
    // .. Porcupine usage here
}
```

## Non-English Wake Words

In order to detect non-English wake words you need to use the corresponding model file. The model files for all supported languages are available [here](/lib/common).

## Demos

The [Porcupine dotnet demo project](/demo/dotnet) is a .NET Core command line application that allows for 
processing real-time audio (i.e. microphone) and files using Porcupine.
