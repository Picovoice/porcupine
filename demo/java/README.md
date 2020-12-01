# Porcupine Wake Word Engine Demos

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

This package contains Java command-line demos for processing real-time audio (i.e. microphone) and audio files
using Porcupine wake word engine.

## Porcupine

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

You can get the latest Java demo executable JARs [here](/demo/java/bin). 

If you wish, you can build the demos from source by opening the project with the [IntelliJ IDE](https://www.jetbrains.com/idea/download/).
Select "Build > Build Project" to build the two demo classes or "Build > Build Artifacts" to create the executable JARs.

## Usage

NOTE: the working directory for java commands is:

```bash
porcupine/demo/java/bin
```

### File Demo

The file demo uses Porcupine to scan for keywords in a wave file. The demo is mainly useful for quantitative performance benchmarking against a corpus of audio data. 
Porcupine processes a 16kHz, single-channel audio stream. If a stereo file is provided it only processes the first (left) channel. 
The following processes a file looking for instances of the phrase "Picovoice":

```bash
java -jar porcupine-file-demo.jar -i ${AUDIO_PATH} -k picovoice
```

`-k` or `--keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string:

```bash
java -jar porcupine-file-demo.jar -h
```

To detect multiple phrases concurrently provide them as separate arguments. If the wake word is more than a single word, surround the argument in quotation marks:

```bash
java -jar porcupine-file-demo.jar -i ${AUDIO_PATH} -k grasshopper "hey siri"
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://picovoice.ai/console/))
use the `-kp` or `--keyword_paths` argument:

```bash
java -jar porcupine-file-demo.jar -i ${AUDIO_PATH} -kp ${KEYWORD_PATH_ONE} ${KEYWORD_PATH_TWO}
```

The sensitivity of the engine can be tuned per keyword using the `-s` or `--sensitivities` input argument:

```bash
java -jar porcupine-file-demo.jar -i ${AUDIO_PATH} -k grasshopper porcupine -s 0.3 0.6
```

Sensitivity is the parameter that enables trading miss rate for the false alarm rate. It is a floating-point number within
`[0, 1]`. A higher sensitivity reduces the miss rate at the cost of increased false alarm rate.

### Microphone Demo

This demo opens an audio stream from a microphone and detects utterances of a given wake word. The following opens the default
microphone and detects occurrences of "Picovoice":

```bash
java -jar porcupine-mic-demo.jar -k picovoice
```

`-k` or `--keywords` is a shorthand for using default keyword files shipped with the package. The list of default keyword files
can be seen in the usage string:

```bash
java -jar porcupine-mic-demo.jar -h
```

To detect multiple phrases concurrently provide them as separate arguments. If the wake word is more than a single word, surround the argument in quotation marks: 

```bash
java -jar porcupine-mic-demo.jar -k picovoice "hey siri"
```

To detect non-default keywords (e.g. models created using [Picovoice Console](https://picovoice.ai/console/))
use the `-kp` or `--keyword_paths` argument:

```bash
java -jar porcupine-mic-demo.jar -kp ${KEYWORD_PATH_ONE} ${KEYWORD_PATH_TWO}
```

It is possible that the default audio input device is not the one you wish to use. There are a couple
of debugging facilities baked into the demo application to solve this. First, type the following into the console:

```bash
java -jar porcupine-mic-demo.jar -sd
```

It provides information about various audio input devices on the box. On a Windows PC, this is the output:

```
Available input devices:

    Device 4: Microphone Array (Realtek(R) Au
    Device 5: Microphone Headset USB
``` 

You can use the device index to specify which microphone to use for the demo. For instance, if you want to use the Headset 
microphone in the above example, you can invoke the demo application as below:

```bash
java -jar porcupine-mic-demo.jar -k picovoice -di 5
```

If the problem persists we suggest storing the recorded audio into a file for inspection. This can be achieved with:

```bash
java -jar porcupine-mic-demo.jar -k picovoice -di 5 -o ./test.wav
```

If after listening to stored file there is no apparent problem detected please open an issue.