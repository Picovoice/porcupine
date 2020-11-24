# Porcupine Wake Word Engine

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. 

Porcupine is:

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient making it perfect for IoT.
- scalable. It can detect multiple always-listening voice commands with no added CPU/memory footprint.
- self-service. Developers can train custom wake phrases using [Picovoice Console](https://picovoice.ai/console/).

## Compatibility

- Java 11+
- Runs on Linux (x86_64), macOS (x86_64) and Windows (x86_64)

## Installation

You can install the Porcupine Java binding by downloading and referencing the latest [Porcupine JAR file](/binding/java/bin/).

## Build

To build from source, we recommend using the [IntelliJ IDE](https://www.jetbrains.com/idea/download/). Open the .iml file with IntelliJ and
click "Build > Build Project" to build or "Build > Build Artifacts" to package as a JAR file.

## Usage

The easiest way to create an instance of the engine is with the Porcupine Builder:

```java
import ai.picovoice.porcupine.*;

try{
    Porcupine handle = new Porcupine.Builder()
                        .setKeyword("picovoice")
                        .build();
} catch (PorcupineException e) { }
```

`handle` is an instance of Porcupine that detects utterances of "Picovoice". The `setKeyword()` builder argument is a shorthand
for accessing default keyword model files shipped with the package. 

The list of default keywords can be retrieved by:

```java
import ai.picovoice.porcupine.*;

for(String keyword : Porcupine.KEYWORDS){
    System.out.println(keyword);
}
```

Porcupine can detect multiple keywords concurrently:

```java
import ai.picovoice.porcupine.*;

try{
    Porcupine handle = new Porcupine.Builder()
                        .setKeywords(new String[]{"bumblebee", "picovoice" }
                        .build();
} catch (PorcupineException e) { }
```

To detect non-default keywords use the `setKeywordPaths()` builder argument instead:

```java
import ai.picovoice.porcupine.*;

String[] keywordPaths = new String[]{ "/absolute/path/to/keyword/one", "/absolute/path/to/keyword/two", ...}
try{
    Porcupine handle = new Porcupine.Builder()
                        .setKeywordPaths(keywordPaths)
                        .build();
} catch (PorcupineException e) { }
```

The sensitivity of the engine can be tuned per-keyword using the `setSensitivities` builder argument

```java
import ai.picovoice.porcupine.*;

try{
    Porcupine handle = new Porcupine.Builder()
                        .setKeywords(new String[]{"grapefruit", "porcupine"})
                        .setSensitivities(new float[]{ 0.6f, 0.35f })
                        .build();
} catch (PorcupineException e) { }
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating-point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

When initialized, the valid sample rate is given by `handle.getSampleRate()`. Expected frame length (number of audio samples
in an input array) is `handle.getFrameLength()`. The engine accepts 16-bit linearly-encoded PCM and operates on
single-channel audio.

```java
short[] getNextAudioFrame(){
    // .. get audioFrame
    return audioFrame;
}

while(true){
    int keywordIndex = handle.process(getNextAudioFrame());
    if(keywordIndex >= 0){
	    // .. detection event logic/callback
    }
}
```

Once you're done with Porcupine, ensure you release its resources explicitly:

```java
handle.delete();
```

## Demos

The [Porcupine Java demo](/demo/java) is a Java command-line application that allows for 
processing real-time audio (i.e. microphone) and files using Porcupine.