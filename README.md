# Porcupine

[![GitHub release](https://img.shields.io/github/release/Picovoice/Porcupine.svg)](https://github.com/Picovoice/Porcupine/releases)

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

Porcupine is a highly-accurate and lightweight **wake word** (a.k.a. **keyword spotting**, **trigger word detection**,
**hotword detection**, and **voice command**) engine. It enables developers to build always-listening voice-enabled
applications. It is

* using **deep neural networks** trained in **real-world situations**.
* compact and computationally-efficient making it suitable for **IoT**. It can run with as low as **20 KB RAM** on an MCU.
* **cross-platform**. It is implemented in fixed-point ANSI C. Currently **Raspberry Pi (all variants)**, **Beagle Bone**,
**Android**, **iOS**, **watchOS**, **Linux (x86_64)**, **Mac**, **Windows**, and **web browsers** (**WebAssembly**) are
supported. Furthermore, Support for various **ARM Cortex-A** and **ARM Cortex-M** processors and **DSP cores** is available
for commercial customers.
* **scalable**. It can detect multiple (possibly many) voice commands concurrently with no added CPU/memory footprint.
* self-service. Developers are empowered to choose from a set of predefined wake phrases on different platforms and use
them for free. In addition, developers can generate custom wake phrases using
[Picovoice Console](https://console.picovoice.ai) (subject to certain limitations and only on Linux, Mac, or Windows)
for non-commercial, personal, and evaluation-only purposes.  

## Picovoice Console

Announcing the [Picovoice Console](https://console.picovoice.ai). The Console is a web-based platform for
building voice applications. You can sign up for an account with your email address or with your GitHub account.

The console succeeds the (now retired) optimizer tool, as it can be used to train custom wake-words (Porcupine .ppn files). If you cloned
this repository prior to August 31st, 2019, we highly recommend to make a new clone and use the Console for training custom
wake-words. The size of the repository is reduced from ~3 GB to 30 MB (100X reduction), which should make working with it easier as well as reduce cloning timeout errors from GitHub that some people have reported.


## Table of Contents

* [Try It Out](#try-it-out)
* [Performance](#performance)
* [Model Variants](#model-variants)
* [Structure of Repository](#structure-of-repository)
* [Running Demo Applications](#running-demo-applications)
    * [Python Demo Application](#python-demo-application)
    * [Android Demo Application](#android-demo-application)
    * [iOS Demo Application](#ios-demo-application)
* [Evaluating Keyword Files](#evaluating-keyword-files)
* [Integration](#integration)
    * [C](#c)
    * [Python](#python)
    * [C#](#csharp)
    * [Android](#android)
    * [iOS](#ios)
    * [Javascript](#javascript)
* [Contributing](#contributing)
* [Releases](#releases)
* [License](#license)
* [FAQ](#faq)

## Try It Out

Try out Porcupine using its [interactive web demo](https://picovoice.ai/demos/porcupine_wasm_smart_lighting.html).
You need a working microphone.

Try out Porcupine by downloading its
[Android demo application](https://play.google.com/store/apps/details?id=ai.picovoice.porcupine.demo&hl=en). The demo
application allows you to test Porcupine on a variety of wake words in any environment.

![Android Demo](resources/images/demo.gif)

Try out Porcupine by installing its [PIP package](https://pypi.org/project/pvporcupine/).

See Porcupine in action on an ARM Cortex-M7 (accompanied by [rhino](https://github.com/Picovoice/rhino) for intent inference).

[![Porcupine in Action](https://img.youtube.com/vi/WadKhfLyqTQ/0.jpg)](https://www.youtube.com/watch?v=WadKhfLyqTQ)

See Porcupine in action on an ARM Cortex-M4 (accompanied by [rhino](https://github.com/Picovoice/rhino) for intent inference).

[![Porcupine in Action](https://img.youtube.com/vi/T0tAnh8tUQg/0.jpg)](https://www.youtube.com/watch?v=T0tAnh8tUQg)

[![Porcupine in Action](https://img.youtube.com/vi/dH8Q-JplpSo/0.jpg)](https://www.youtube.com/watch?v=dH8Q-JplpSo)

## Performance

A comparison between accuracy and runtime metrics of Porcupine and two other widely-used libraries, PocketSphinx and
Snowboy, is provided [here](https://github.com/Picovoice/wakeword-benchmark). Compared to the best-performing engine of these two,
Porcupine's standard model is 3.34 times more accurate, 4.38 times faster (on Raspberry Pi 3). 

## Model Variants

Porcupine has two flavours: **standard** and **compressed**. The compressed model is specifically designed
for deeply-embedded applications (MCUs and DSPs). Its accuracy is slightly lower than the standard model, but it consumes
considerably less resources. Below is the comparison of runtime measurements for different variants of Porcupine on
Raspberry Pi3:

| Model Variant | CPU Usage | Model Size (KB)
:---: | :---: | :---:
Standard | 5.67% | 1388
Compressed | 2.43% | 232

For an accuracy comparison of different variants, refer to
[benchmark repository](https://github.com/Picovoice/wakeword-benchmark).

## Structure of Repository

Porcupine is shipped as an ANSI C precompiled library. The binary files for supported platforms are located under
[lib/](/lib) and header files are at [include/](/include). Currently, BeagleBone, Raspberry Pi, Android, iOS, watchOS,
Linux, macOS, Windows, and modern web browsers (with WebAssembly) are supported.

Bindings are available at [binding/](/binding) to facilitate usage from higher-level languages and platforms. Demo
applications are located at [demo/](/demo). We recommend using one of the demo applications as a starting point for your own
implementation, when possible.

Finally, [resources/](/resources) is a placeholder for data used by various applications within the repository.

Below is a quick walkthrough of the repository. For detailed instructions please visit the relevant pages. Throughout the
documentation, it is assumed that the current working directory is the root of the repository.

## Running Demo Applications

### Python Demo Application

### PIP

Install Porcupine [PIP package](https://pypi.org/project/pvporcupine/). Then with a working  microphone connected to
your device run the following in the terminal

```shell
pvporcupine_mic --keywords porcupine
``` 

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of wake-word "porcupine".

In order to process audio files (e.g. WAV or FLAC) run

```shell
pvporcupine_file --input_audio_file_path ${PATH_TO_AN_AUDIO_FILE} --keywords bumblebee
``` 

Then the engine scans the given audio file for occurrences of keyword "bumblebee".

### Repository

This [demo application](/demo/python) allows testing Porcupine using your computer's microphone. It opens an input audio
stream, monitors it using Porcupine's library, and logs the detection events into the console. Below is an example of
running the demo for hotword `picovoice` from the command line. Replace `${SYSTEM}` with the name of the operating system
on your machine (e.g. linux, mac, windows, or raspberrypi).

```bash
python demo/python/porcupine_demo.py --keyword_file_paths resources/keyword_files/${SYSTEM}/alexa_${SYSTEM}.ppn
```

### Android Demo Application

Using [Android Studio](https://developer.android.com/studio/index.html), open [demo/android](/demo/android) as an Android
project and then run the application. You will need an Android device (with developer options enabled) connected to
your machine.

### iOS Demo Application

Using [Xcode](https://developer.apple.com/xcode/), open [demo/ios](/demo/ios) and run the application. You will need
an iOS device connected to your machine and a valid Apple developer account.

## Evaluating Keyword Files

Porcupine enables developers to evaluate models for any wake word. This is done using
[Picovoice's console](https://console.picovoice.ai/). You may use the console to train wake word models for execution on
Linux (x86_64), Mac, or Windows and only for non-commercial and personal use.

## Integration

Below are code snippets showcasing how Porcupine can be integrated into different applications.

### C

Porcupine is implemented in ANSI C and therefore can be directly linked to C applications.
[include/pv_porcupine.h](/include/pv_porcupine.h) header file contains relevant information. An instance of Porcupine
object can be constructed as follows.

```c
const char *model_file_path = ... // The file is available at lib/common/porcupine_params.pv
const char *keyword_file_path = ...
const float sensitivity = 0.5;

pv_porcupine_object_t *handle;

const pv_status_t status = pv_porcupine_init(model_file_path, keyword_file_path, sensitivity, &handle);

if (status != PV_STATUS_SUCCESS) {
    // error handling logic
}
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate (false reject rate) at cost of increased false alarm rate.

Now the `handle` can be used to monitor incoming audio stream. Porcupine accepts single channel, 16-bit PCM audio.
The sample rate can be retrieved using `pv_sample_rate()`. Finally, Porcupine accepts input audio in consecutive chunks
(aka frames) the length of each frame can be retrieved using `pv_porcupine_frame_length()`.

```c
extern const int16_t *get_next_audio_frame(void);

while (true) {
    const int16_t *pcm = get_next_audio_frame();
    bool result;
    const pv_status_t status = pv_porcupine_process(handle, pcm, &result);
    if (status != PV_STATUS_SUCCESS) {
        // error handling logic
    }
    if (result) {
        // detection event logic/callback
    }
}
```

Finally, when done be sure to release the acquired resources.

```c
pv_porcupine_delete(handle);
```

### Python

#### PIP

The PIP package exposes a factory method to create instances of the engine as below

```python
import pvporcupine

handle = pvporcupine.create(keywords=['picovoice', 'bumblebee'])
```

`keywords` argument is a shorthand for accessing default keyword files shipped with the library. The default keyword files
available can be retrieved via

```python
import pvporcupine

print(pvporcupine.KEYWORDS)
```

If you wish to use a non-default keyword file you need to identify its path as below

```python
import pvporcupine

handle = pvporcupine.create(keyword_file_paths=['path/to/non/default/keyword/file'])
```

In order to learn how to use the created object continue reading the section below.

#### Repository

[/binding/python/porcupine.py](/binding/python/porcupine.py) provides a Python binding for Porcupine library. Below is a
quick demonstration of how to construct an instance of it to detect multiple keywords concurrently.

```python
library_path = ... # Path to Porcupine's C library available under lib/${SYSTEM}/${MACHINE}/
model_file_path = ... # It is available at lib/common/porcupine_params.pv
keyword_file_paths = ['path/to/keyword/1', 'path/to/keyword/2', ...]
sensitivities = [0.5, 0.4, ...]
handle = Porcupine(library_path, model_file_path, keyword_file_paths=keyword_file_paths, sensitivities=sensitivities)
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

When initialized, valid sample rate can be obtained using ```handle.sample_rate```. Expected frame length
(number of audio samples in an input array) is ```handle.frame_length```. The object can be used to monitor
incoming audio as below.

```python
def get_next_audio_frame():
    pass

while True:
    pcm = get_next_audio_frame()
    keyword_index = handle.process(pcm)
    if keyword_index >= 0:
        # detection event logic/callback
        pass
```

Finally, when done be sure to explicitly release the resources as the binding class does not rely on the garbage
collector.

```python
handle.delete()
```

### csharp

[/binding/dotnet/PorcupineCS/Porcupine.cs](/binding/dotnet/PorcupineCS/Porcupine.cs) provides a c# binding for Porcupine
. Below is a quick demonstration of how to construct an instance of it to detect multiple keywords concurrently.


```csharp
string model_file_path = ... // The file is available at lib/common/porcupine_params.pv
string keyword_file_path = ...
float sensitivity = 0.5;
Porcupine instance;

instance = new Porcupine(model_file_path, keyword_file_path, sensitivity);

if (instance.Status != PicoVoiceStatus.SUCCESS) {
    // error handling logic
}
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

Now the `instance` can be used to monitor incoming audio stream. Porcupine accepts single channel, 16-bit PCM audio.
The sample rate can be retrieved using `instance.SampleRate()`. Finally, Porcupine accepts input audio in consecutive chunks
(aka frames) the length of each frame can be retrieved using `instance.FrameLength()`.

```csharp
Int16[] GetNextAudioFrame()
{
    ... // some functionality that gets the next frame
}


while (true) {
    Int16[] frame = GetNextAudioFrame();
    bool result;
    PicoVoiceStatus status = instance.Process(pcm, out result);
    if (status != PicoVoiceStatus.SUCCESS) {
        // error handling logic
    }
    if (result) {
        // detection event logic/callback
    }
}
```

Finally, when done we don't need to release the resources ourselves; the garbage collector will handle this.
But, if you want to do it yourself:

```csharp
instance.Dispose();
```

### Android

There are two possibilities for integrating Porcupine into an Android application.

#### Binding

[Porcupine](/binding/android/Porcupine/app/src/main/java/ai/picovoice/porcupine/Porcupine.java) provides a binding for
Android using [JNI](https://docs.oracle.com/javase/7/docs/technotes/guides/jni/). It can be initialized using.

```java
    final String modelFilePath = ... // It is available at lib/common/porcupine_params.pv
    final String keywordFilePath = ...
    final float sensitivity = 0.5f;

    Porcupine porcupine = new Porcupine(modelFilePath, keywordFilePath, sensitivity);
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

Once initialized, ```porcupine``` can be used to monitor incoming audio.

```java
    private short[] getNextAudioFrame();

    while (true) {
        final boolean result = porcupine.process(getNextAudioFrame());
        if (result) {
            // detection event logic/callback
        }
    }
```

Finally, be sure to explicitly release resources acquired by porcupine as the class does not rely on the garbage collector
for releasing native resources.

```java
    porcupine.delete();
```

#### High-Level API

[PorcupineManager](binding/android/PorcupineManager/app/src/main/java/ai/picovoice/porcupinemanager/PorcupineManager.java)
 provides a high-level API for integrating Porcupine into Android applications. It manages all activities related to creating
 an input audio stream, feeding it into the Porcupine library, and invoking a user-provided detection callback. The class
 can be initialized as below.

```java
    final String modelFilePath = ... // It is available at lib/common/porcupine_params.pv
    final String keywordFilePath = ...
    final float sensitivity = 0.5f;

    PorcupineManager manager = new PorcupineManager(
            modelFilePath,
            keywordFilePath,
            sensitivity,
            new KeywordCallback() {
                @Override
                public void run() {
                    // detection event logic/callback
                }
            });
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

When initialized, input audio can be monitored using `manager.start()`. When done be sure to stop the manager using
`manager.stop()`.

### iOS

There are two approaches for integrating Porcupine into an iOS application.

#### Direct

Porcupine is shipped as a precompiled ANSI C library and can directly be used in Swift using module maps. It can be
initialized to detect multiple wake words concurrently using:

```swift
let modelFilePath: String = ... // It is available at lib/common/porcupine_params.pv
let keywordFilePaths: [String] = ["path/to/keyword/1", "path/to/keyword/2", ...]
let sensitivities: [Float] = [0.3, 0.7, ...];
var handle: OpaquePointer?

let status = pv_porcupine_multiple_keywords_init(
    modelFilePath,
    Int32(keywordFilePaths.count), // Number of different keywords to monitor for
    keywordFilePaths.map{ UnsafePointer(strdup($0)) },
    sensitivities,
    &handle)
if status != PV_STATUS_SUCCESS {
    // error handling logic
}
```

Then `handle` can be used to monitor incoming audio stream.

```swift
func getNextAudioFrame() -> UnsafeMutablePointer<Int16> {
    //
}

while true {
    let pcm = getNextAudioFrame()
    var keyword_index: Int32 = -1

    let status = pv_porcupine_multiple_keywords_process(handle, pcm, &keyword_index)
    if status != PV_STATUS_SUCCESS {
        // error handling logic
    }
    if keyword_index >= 0 {
        // detection event logic/callback
    }
}
```

When finished, release the resources via

```swift
    pv_porcupine_delete(handle)
```

#### Binding

The [PorcupineManager](/binding/ios/PorcupineManager.swift) class manages all activities related to creating an input audio
stream, feeding it into Porcupine's library, and invoking a user-provided detection callback. The class can be
initialized as below:

```swift
let modelFilePath: String = ... // It is available at lib/common/porcupine_params.pv
let keywordCallback: ((WakeWordConfiguration) -> Void) = {
    // detection event callback
}

let wakeWordConfiguration1 = WakeWordConfiguration(name: "1", filePath: "path/to/keyword/1", sensitivity: 0.5)
let wakewordConfiguration2 = WakeWordConfiguration(name: "2", filePath: "path/to/keyword/2", sensitivity: 0.7)
let configurations = [ wakeWordConfiguration1, wakewordConfiguration2 ]

let manager = try PorcupineManager(modelFilePath: modelFilePath, wakeKeywordConfigurations: configurations, onDetection: keywordCallback)
```

When initialized, input audio can be monitored using `manager.startListening()`. When done be sure to stop the manager using
`manager.stopListening()`.

### Javascript

Porcupine is available on modern web browsers in [WebAssembly](https://webassembly.org/). The [Javascript binding](/binding/js/porcupine/porcupine.js)
makes it trivial use Porcupine within a Javascript environment. Instantiate a new instance of engine using the factory method
as below

```javascript
    let keywordIDs = Array(UInt8Array(), ...);
    let sensitivities = Float32Array(...);
    let obj = Porcupine.create(keywordIDs, sensitivities);
```

when initialized incoming audio stream can be processed using the `process` method. Be sure to release the resources
acquired by WebAssembly using `.release` when done

```javascript
    while (true) {
        obj.process(audioFrameInt16Array);
    }
    
    // release when done
    obj.release();
```

For more information, refer to [binding](/binding/js) and [demo](/demo/js).

## Contributing

If you would like to contribute to Porcupine, please read through [CONTRIBUTING.md](CONTRIBUTING.md).

### Acknowledgements

* Thank you @charithe for Go binding/demo.
* Thank you @HeadhunterXamd for C Sharp binding/demo.
* Thank you @oziee for adding C++ ALSA demo.
* Thank you @herlihalim for refactoring iOS binding and demo.
* Thank you @veeableful for adding C++ and Rust demo.
* Thank you @fquirin for adding non-blocking Python demo.
* Thank you @dyah10 for adding watchOS binding and demo.

## Releases

### v1.6.0 - April 25th, 2019

* Improved accuracy across all models.
* Runtime optimization across all models
* Added support for Beagle Bone
* iOS build can run on simulator now.

### v1.5.0 - November 13, 2018

* Improved optimizer's accuracy.
* Runtime optimization.
* Added support for running within web browsers (WebAssembly).

### v1.4.0 - July 20, 2018

* Improved accuracy across all models (specifically compressed variant).
* Runtime optimizations.
* Updated documentation.

### v1.3.0 - June 19, 2018

* Added compressed model (200 KB) for deeply-embedded platforms.
* Improved accuracy.
* Runtime optimizations and bug fixes.

### v1.2.0 - April 21, 2018

* Runtime optimizations across platforms.
* Added support for watchOS.

### v1.1.0 - April 11, 2018

* Added multiple command detection capability. Porcupine can now detect multiple commands with virtually no added
CPU/memory footprint.

### v1.0.0 - March 13, 2018

* Initial release.

## License

This repository is licensed under Apache 2.0. This allows running the Porcupine wake word detection library on all
supported platforms using the set of freely-available [keyword files](/resources/keyword_files).

You may create custom wake-word models (for execution on Linux, Mac, and Windows) using
[Picovoice Console](https://console.picovoice.ai/) for non-commercial and personal use free of charge.

Custom wake-words for other platforms must be generated by the Picovoice engineering team and are only provided with the
purchase of the Picovoice development or commercial license. To enquire about the Picovoice development and commercial
license terms and fees, [contact us](https://picovoice.ai/contact.html).

## FAQ

**[Q] Which Picovoice speech product should I use?**

**[A]** If you need to recognize a single phrase or a number of predefined phrases (dozens or fewer), in an
always-listening fashion, then you should use Porcupine (wake word engine). If you need to recognize complex voice
commands within a confined and well-defined domain with limited number of vocabulary and variations of spoken forms
(1000s or fewer), then you should use [Rhino](https://github.com/Picovoice/rhino) (speech-to-intent engine). If you need
to transcribe free-form speech in an open domain, then you should use [Cheetah](https://github.com/Picovoice/cheetah)
(speech-to-text engine).

**[Q] What are the benefits of implementing voice interfaces on-device, instead of using cloud services?**

**[A]** Privacy, minimal latency, improved reliability, runtime efficiency, and cost savings, to name a few. More detail is
available [here](https://picovoice.ai/blog/the_case_for_voice_ai_on_the_edge.html).

**[Q] Does Picovoice technology work in far-field applications?**

**[A]** It depends on many factors including the distance, ambient noise level, reverberation (echo), quality of
microphone, and audio frontend used (if any). It is recommended to try out our technology using the freely-available
sample models in your environment. Additionally, we often publish open-source benchmarks of our technology in noisy
environments [1](https://github.com/Picovoice/wakeword-benchmark)
[2](https://github.com/Picovoice/speech-to-intent-benchmark) [3](https://github.com/Picovoice/stt-benchmark). If the
target environment is noisy and/or reverberant and the user is few meters away from the microphone, a multi-microphone audio
frontend can be beneficial.

**[Q] Does Picovoice software work in my target environment and noise conditions?**

**[A]** It depends on variety of factors. You should test it out yourself with the free samples made available on
Picovoice GitHub pages. If it does not work, we can fine-tune it for your target environment.

**[Q] Does Picovoice software work in presence of noise and reverberation?**

**[A]** Picovoice software is designed to function robustly in presence of noise and reverberations. We have benchmarked
and published the performance results under various noisy conditions [1](https://github.com/Picovoice/wakeword-benchmark)
[2](https://github.com/Picovoice/speech-to-intent-benchmark) [3](https://github.com/Picovoice/stt-benchmark).
The end-to-end performance depends on the type and amount of noise and reverberation. We highly recommend testing out
the software using freely-available models in your target environment and application.

**[Q] Can I use Picovoice software for telephony applications?**

**[A]** We expect audio with 16000Hz sampling rate. PSTN networks usually sample at 8000Hz. It is possible to
upsample, but then the frequency content above 4000Hz is missing and performance will be suboptimal. It is possible to train
acoustic models for telephony applications, if the commercial opportunity is justified.

**[Q] My audio source is 48kHz/44.1KHz. Does Picovoice software support that?**

**[A]** Picovoice software expects a 16000Hz sampling rate. You will need to resample (downsample). Typically,
operating systems or sound cards (Audio codecs) provide such functionality; otherwise, you will need to implement it.

**[Q] Can Picovoice help with building my voice enabled product?**

**[A]** Our core business is software licensing. That being said, we do have a wide variety of expertise internally
in voice, software, and hardware. We consider such requests on a case-by-case basis and assist clients who can
guarantee a certain minimum licensing volume.

**[Q] If I am using GitHub to evaluate the software, do you provide technical support?**

**[A]** Prior to commercial engagement, basic support solely pertaining to software issues or bugs is provided via
GitHub issues by the open-source community or a member of our team. We do not offer any free support with integration
or support with any platform (operating system or hardware) that is not officially supported via GitHub.

**[Q] Why does Picovoice have GitHub repositories?**

**[A]** To facilitate performance evaluation, for commercial prospects, and to enable the open source community to use the technology for personal and non-commercial applications.

**[Q] What is the engagement process?**

**[A]** You may use what is available on GitHub while respecting its governing license terms, without engaging with us.
This facilitates initial performance evaluation. Subsequently, you may acquire a development license to get access to custom speech models or use the software for development and internal evaluation within a company; the development license is for
building a proof-of-concept or prototype. When ready to commercialize your product, you need to acquire a commercial license.

**[Q] Does Picovoice offer AEC, VAD, noise suppression, or microphone array beamforming?**

**[A]** No. But we do have partners who provide such algorithms. Please add this to your inquiry when reaching out
and we can help to connect you.

**[Q] Can you build a voice-enabled app for me?**

**[A]** We do not provide software development services, so most likely the answer is no. However, via a professional
services agreement we can help with proofs-of-concept (these will typically be rudimentary apps focused on voice user
interface or building the audio pipeline), evaluations on a specific domain/task, integration of SDKs in your app,
training of custom acoustic and language models, and porting to custom hardware platforms.

**[Q] How do I evaluate Porcupine software performance?**

**[A]** We have benchmarked the performance of Porcupine software rigorously and published the results
[here](https://github.com/Picovoice/wakeword-benchmark). We have also open-sourced the code and audio files used for
benchmarking on the same repository to make it possible to reproduce the results. You can also use the code with
your own audio files (noise sources collected from your target environment or utterances of your own wake word) to
benchmark the performance. Additionally, we have made a set of sample wake words freely available on this GitHub
repository on all platforms to facilitate evaluation, testing, and integration.

**[Q] Can Porcupine wake word detection software detect non-English keywords?**

**[A]** It depends. If English speakers can easily pronounce the non-English wake word,
then we can most likely generate it for you. We recommend sending us a few audio samples including the utterance of the
requested wake word so that our engineering team can review and provide feedback on feasibility.

**[Q] What is Porcupine’s wake word detection accuracy?**

**[A]** We have extensive benchmarking on Porcupine performance compared accuracy against alternatives,
and published the result [here](https://github.com/Picovoice/wakeword-benchmark). Porcupine can achieve 90%+ accuracy
(detection rate) with less than 1 false alarm in 8 hours in the presence of ambient noise with 10dB SNR at microphone
level.

**[Q] Can Porcupine detect the wake word if the speaker is yelling/shouting in anger, excitement, or pain?**

**[A]** Porcupine does not have a profile to recognize emotionally-coloured utterances such as yelling, dragging,
mumbling, etc. We do require the speaker to somewhat clearly vocalize the phrase.

**[Q] Does Porcupine’s detection accuracy depend on the choice of wake word?**

**[A]** Generally speaking yes, however it is difficult to quantify the cause-and-effect accurately. We have published
a guide [here](https://picovoice.ai/docs/choose-wake-word/index.html) to help you pick a wake word that would achieve
optimal performance. You will need to avoid using short phrases, and make sure your wake word includes diverse sounds
and at least six phonemes. Long phrases are also not recommended due to the poor user experience.

**[Q] Is there a guideline for picking a wake word?**

**[A]** We have published a guide [here](https://picovoice.ai/docs/choose-wake-word/index.html) to help you pick a
wake word that would achieve optimal performance.

**[Q] How much CPU and memory does Picovoice wake word detection software consume?**

**[A]** We offer several trims for our wake word detection model. The standard model, which is recommended on most
platforms, uses roughly 1.5MB of readonly memory (ROM / FLASH) and 5% of a single core on a Raspberry Pi 3. 

**[Q] What should I set the sensitivity value to?**

**[A]** You should pick a sensitivity parameter that suits your application requirements. A higher sensitivity value
gives a lower miss rate at the expense of higher false alarm rate. If your application places tighter requirements on
false alarms, but can tolerate misses, then you should lower the sensitivity value.  

**[Q] What is an ROC curve?**

**[A]** The accuracy of a binary classifier (any decision-making algorithm with a “yes” or “no” output) can be measured
by two parameters: false rejection rate (FRR) and false acceptance rate (FAR). A wake word detector is a binary
classifier. Hence, we use these metrics to benchmark it.

The detection threshold of binary classifiers can be tuned to balance FRR and FAR. A lower detection threshold yields
higher sensitivity. A highly sensitive classifier has a high FAR and low FRR value (i.e. it accepts almost everything).
A receiver operating characteristic (ROC) curve plots FRR values against corresponding FAR values for varying
sensitivity values 

To learn more about ROC curves and benchmarking a wake word detection, you may read the blog post
[here](https://picovoice.ai/blog/benchmarking_wake_word_engines.html) and Porcupine benchmark published
[here](https://github.com/Picovoice/wakeword-benchmark). 

**[Q] If I use Porcupine wake word detection in my mobile application, does it function when the app is running in the background?**

**[A]** Developers have been able to successfully run Porcupine wake word detection software on iOS and Android in
background mode. However, this feature is controlled by the operating system, and we cannot guarantee that this will
be possible in future releases of iOS or Android. Please check iOS and Android guidelines, technical documentation,
and terms of service before choosing to run Porcupine wake word detection in the background. We recommend using the
sample demo applications made available on this repository to test this capability in your end application before
acquiring a development or commercial license.

**[Q] Which platforms does Porcupine wake word detection support?**

**[A]** Porcupine wake word detection software is supported on Raspberry Pi (all models), BeagleBone, Android, iOS,
Linux (x86_64), macOS, Windows, and modern web browsers (excluding Internet Explorer). Additionally, we have support
for various ARM Cortex-A and ARM Cortex-M (M4/M7) MCUs by NXP and STMicro. 

**[Q] What is required to support additional languages?**

**[A]** Porcupine is architected to work with any language, and there are no technical limitations on supporting most
languages. However, supporting a new language requires significant effort and investment. The undertaking is a business
decision which depends on our current priorities, pipeline, and the scale of commercial opportunity for which the
language support is required.

**[Q] Does Porcupine wake word detection software work with everyone’s voice (universal) or does it only work with my voice (personal)?**

**[A]** Porcupine wake word detection software is universal and trained to work with a variety of accents and people’s
voices.

**[Q] Does Porcupine wake word detection work with children’s voices?**

**[A]** Porcupine may not work well with very young children as their voices are different from adult voices. We have
made the software available for free evaluation with a set of sample wake words. We recommend that you test the
engine with speech of children within your target age range before acquiring a development or commercial license.

**[Q] Do users need to pause and remain silent before saying the wake word?**

**[A]** By default, no. But if that is a requirement, we can customize the software
(as part of our professional services for you) to require silence either before or after the wake word.

**[Q] If my wake phrase is made of two words (e.g., “Hey Siri”), does the software detect if the user inserts silence/pause in between each word?**

**[A]** By default, the engine ignores silence in between the words. However, if that is a requirement, we can
customize the software (as part of our standard professional services) to require silence between each word.

**[Q] Our marketing team is having difficulty deciding on the choice for wake word, can you help?**

**[A]** Yes, we can help you with the process of choosing the right wake word for your brand. We also offer the option
for revision if you change your mind after the purchase of a development license.

**[Q] Does Porcupine wake word detection work with accents?**

**[A]** Yes, it works generally well with accents. However, it’s impossible to objectively quantify it. We recommend you
try the engine for yourself and perhaps evaluate with an accented dataset of your choice to see if it meets your
requirements.

**[Q] How does Picovoice wake word detection software work when UK and US wake word pronunciations sometimes differ?**

**[A]** For words that have different pronunciations in UK and US English, like “tomato”, we recommend listening
for both pronunciations simultaneously with two separate wake word model files, each targeting a distinct pronunciation.

**[Q] How many wake words can Porcupine detect simultaneously?**

**[A]** There is no technical limit on the number of wake words the software can listen to simultaneously. 

**[Q] How much additional memory and CPU is needed for detecting additional wake word or trigger phrases?**

**[A]** Listening to additional wake words does not increase the CPU usage. However it will require 1 KB of
memory per additional wake word model.

**[Q] Is the Picovoice “Alexa” wake word verified by Amazon?**

**[A]** Amazon Alexa Certification requirements are different for near, mid, and far-field applications
(AVS, AMA, etc.). Also, the certification is typically performed on the end hardware, and the outcome depends on many
design choices such as microphone, enclosure acoustics, audio front end, and wake word. Picovoice can assist with new
product introduction (NPI) and Alexa certification under our technical support package.

**[Q] Does Picovoice wake word detection software work with Google Assistant?**

**[A]** Yes. However, your product may have to go through a certification procedure with Google. Please check Google’s
guidelines and terms of service for related information.

**[Q] Can you use Picovoice wake word detection software with Cortana, IBM Watson, or Samsung Bixby?**

**[A]** Yes, Picovoice can generate any third-party wake words at your request. However, you are responsible for any
necessary integration with such platforms and potential areas of compliance.

**[Q] What’s the power consumption of Picovoice wake word detection engine?**

**[A]** The absolute power consumption (in wattage) depends on numerous factors such as processor architecture,
vendor, fabrication technology, and system level power management design. If your design requires low power
consumption in the (sub) milliwatt range for always-listening wake word detection, you will likely need to consider
MCU (ARM Cortex-M) or DSP implementation.

**[Q] Can Porcupine distinguish words with similar pronunciation?**

**[A]** The rigidity of rejecting words with similar pronunciation would have several side effects such as rejecting
accented pronunciations, as well as higher rejection rate in noisy conditions. By lowering the detection
sensitivity you can achieve lower false acceptance of words with similar pronunciations at the cost of higher miss rate.

**[Q] How can I run Picovoice software on my ARM-based MPU running a Yocto customized embedded Linux?**

**[A]** As part of our standard professional services, we can port our software to custom platforms for a one-time
engineering fee and prepaid license royalties. We review these on a case-by-case basis and provide a quotation based
on the complexity and type of the platform. Please note that the port must be performed in-house by our engineering
team, since it requires direct access to our IP, proprietary technology, and toolchains. We would also require at
least one development board running your target OS to perform this task.

**[Q] What is your software licensing model?**

**[A]** The software published on this repository is available under Apache 2.0. If you need custom wake word models on
a specific platform for commercial development (building PoC, prototyping, or product development) you need to acquire a
development license. To install and use Picovoice software on commercial products with custom wake word models you need
to acquire a commercial license. If you are developing a product within a company and working towards commercialization
please reach out to us to acquire the appropriate license by filling out the form
[here](https://picovoice.ai/contact.html).

**[Q] Can I use wake word models generated by the [Picovoice Console](https://console.picovoice.ai) in a commercial
product?**

**[A]** The Picovoice Console and keyword files it generates can only be used for non-commercial and evaluation purposes.
If you are developing a commercial product, you must acquire a development license. To acquire a development license 
fill out the form [here](https://picovoice.ai/contact.html).
