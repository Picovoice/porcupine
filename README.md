# Porcupine

[![GitHub release](https://img.shields.io/github/release/Picovoice/Porcupine.svg)](https://github.com/Picovoice/Porcupine/releases)

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient. It is perfect for IoT.
- cross-platform. Raspberry Pi, BeagleBone, Android, iOS, Linux (x86_64), macOS (x86_64), Windows (x86_64), and web
browsers are supported. Additionally, enterprise customers have access to ARM Cortex-M SDK.
- scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
- self-service. Developers can train custom wake word models using [Picovoice Console](https://picovoice.ai/console/).

## Table of Contents

- [Porcupine](#porcupine)
  - [Table of Contents](#table-of-contents)
  - [License & Terms](#license--terms)
  - [Use Cases](#use-cases)
  - [Try It Out](#try-it-out)
  - [Performance](#performance)
  - [Demos](#demos)
    - [Python](#python-demos)
    - [.NET](#net-demos)
    - [Java](#java-demos)
    - [React Native](#react-native-demos)
    - [Android](#android-demos)
    - [iOS](#ios-demos)
    - [JavaScript](#javascript-demos)
    - [NodeJS](#nodejs-demos)
    - [C](#c-demos)
  - [SDKs](#sdks)
    - [Python](#python)
    - [.NET](#net)
    - [Java](#java)
    - [React Native](#react-native)
    - [Android](#android)
    - [iOS](#ios)
    - [JavaScript](#javascript)
    - [NodeJS](#nodejs)
    - [C](#c)
  - [Releases](#releases)
  - [FAQ](#faq)

## License & Terms

Porcupine SDK is free and licensed under Apache 2.0 including the [models](/resources/keyword_files) released within the
repository. [Picovoice Console](https://picovoice.ai/console/) offers two types of subscriptions: Personal and Enterprise.
Personal accounts can train custom wake word models that run on x86_64, subject to limitations and strictly for
non-commercial purposes. Personal accounts empower researchers, hobbyists, and tinkerers to experiment. Enterprise
accounts can unlock all capabilities of Picovoice Console, are permitted for use in commercial settings, and have a path
to graduate to commercial distribution[<sup>*</sup>](https://picovoice.ai/pricing/).

## Use Cases

Porcupine is the right product if you need to detect one or a few static (always-listening) voice commands.

- If you want to create voice experiences similar to Alexa or Google, checkout
[Picovoice platform](https://github.com/Picovoice/picovoice).
- If you need to understand complex and naturally-spoken voice commands within a specific domain, check out
[Rhino Speech-to-Intent engine](https://github.com/Picovoice/rhino).

## Try It Out

- [Interactive Web Demo](https://picovoice.ai/demos/lamp/)

- [Android Demo Application](https://play.google.com/store/apps/details?id=ai.picovoice.porcupine.demo&hl=en)

- Porcupine and [Rhino](https://github.com/Picovoice/rhino) on an ARM Cortex-M7

[![Porcupine in Action](https://img.youtube.com/vi/WadKhfLyqTQ/0.jpg)](https://www.youtube.com/watch?v=WadKhfLyqTQ)

## Performance

A comparison between accuracy and runtime metrics of Porcupine and two other widely-used libraries, PocketSphinx and
Snowboy, is provided [here](https://github.com/Picovoice/wakeword-benchmark). Compared to the best-performing engine of
these two, Porcupine is **5.4 times more accurate** and **6.5 times faster** (on Raspberry Pi 3).

## Demos

### Python Demos

Install [PyAudio](https://people.csail.mit.edu/hubert/pyaudio/) and then the demo package:

```bash
sudo pip3 install pvporcupinedemo
```

With a working microphone connected to your device run the following in the terminal:

```shell
porcupine_demo_mic --keywords porcupine
```

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of `Porcupine`.

For more information about Python demos go to [demo/python](/demo/python).

### .NET Demos

Install [OpenAL](https://openal.org/) and then from [demo/dotnet/PorcupineDemo](/demo/dotnet/PorcupineDemo) run the
following in the terminal to build the demo:

```bash
dotnet build -c MicDemo.Release
```

Make sure there is a working microphone connected to your device. From [demo/dotnet/PorcupineDemo](/demo/dotnet/PorcupineDemo) run the
following in the terminal:

```bash
dotnet run -c MicDemo.Release -- --keywords porcupine
```

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of `Porcupine`.

For more information about .NET demos go to [demo/dotnet](/demo/dotnet).

### Java Demos

Make sure there is a working microphone connected to your device. From the root of the repository run the following in
the terminal:

```bash
java -jar demo/java/bin/porcupine-mic-demo.jar -k porcupine
```

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of `Porcupine`.

For more information about Java demos go to [demo/java](/demo/java).

### React Native Demos

To run the React Native Porcupine demo app you'll first need to setup your React Native environment. For this, 
please refer to [React Native's documentation](https://reactnative.dev/docs/environment-setup). Once your environment has 
been set up, navigate to [demo/react-native](/demo/react-native) to run the following commands: 

For Android:
```sh
yarn android-install    # sets up environment
yarn android-run        # builds and deploys to Android
```

For iOS:
```sh
yarn ios-install        # sets up environment
yarn ios-run            # builds and deploys to iOS
```

### Android Demos

Using [Android Studio](https://developer.android.com/studio/index.html), open
[demo/android/Activity](/demo/android/Activity) as an Android project and then run the application.

To learn about how to use Porcupine in long running services go to [demo/android/Service](/demo/android/Service).

### iOS Demos

Using [Xcode](https://developer.apple.com/xcode/), open [PorcupineDemo](/demo/ios/PorcupineDemo.xcodeproj) and run the
application.

### JavaScript Demos

From [demo/javascript](/demo/javascript) run the following in the terminal:

```bash
yarn
yarn copy
yarn start
```

If you prefer to use NPM instead, use the following:

```bash
npm install
npm install -g copy-files-from-to
copy-files-from-to
npx serve
```

It will launch a local server running the demo. Open `http://localhost:5000` in your web browser and follow the
instructions on the page.

### NodeJS Demos

Install [node-record-lpcm16](https://www.npmjs.com/package/node-record-lpcm16) NPM package and follow the instructions
there for setting up your microphone. Then install the demo package:

```bash
yarn global add @picovoice/porcupine-node-demo
```

With a working microphone connected to your device run the following in the terminal:

```shell
ppn-mic-demo --keywords porcupine
```

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of `Porcupine`.

For more information about NodeJS demos go to [demo/nodejs](/demo/nodejs).

### C Demos

[Microphone demo](/demo/c/porcupine_demo_mic.c) runs on Linux-based systems (e.g. Ubuntu, Raspberry Pi, and BeagleBone).
Build the demo:

```bash
gcc -std=c99 -O3 -o demo/c/porcupine_demo_mic \
-I include/ demo/c/porcupine_demo_mic.c -ldl -lasound
```

Find the name of audio input device (microphone) on your computer using `arecord -L` and then from the root of the
repository run the demo:

```bash
./demo/c/porcupine_demo_mic ${LIBRARY_PATH} lib/common/porcupine_params.pv \
resources/keyword_files/${SYSTEM}/porcupine_${SYSTEM}.ppn 0.5 ${INPUT_AUDIO_DEVICE}
```

Replace `${LIBRARY_PATH}` with path to appropriate library available under [lib](/lib), `${SYSTEM}` with the
name of the platform you are running on (`linux`, `raspberry-pi`, or `beaglebone`), and `${INPUT_AUDIO_DEVICE}` with
the name of your microphone device. The demo opens an audio stream and detects utterances of `Porcupine`.

For more information about C demos go to [demo/c](/demo/c).

## SDKs

### Python

Install the Python SDK:

```bash
pip3 install pvporcupine
```

The SDK exposes a factory method to create instances of the engine:

```python
import pvporcupine

handle = pvporcupine.create(keywords=['picovoice', 'bumblebee'])
```

`keywords` argument is a shorthand for accessing default keyword files shipped with the library. The default keyword
files available can be retrieved via

```python
import pvporcupine

print(pvporcupine.KEYWORDS)
```

If you wish to use a non-default keyword file you need to identify its path:

```python
import pvporcupine

handle = pvporcupine.create(keyword_paths=['path/to/non/default/keyword/file'])
```

When initialized, valid sample rate can be obtained using `handle.sample_rate`. Expected frame length
(number of audio samples in an input array) is `handle.frame_length`. The object can be used to monitor
incoming audio as follows:

```python
import pvporcupine

handle = pvporcupine.create(keywords=['porcupine'])

def get_next_audio_frame():
    pass

while True:
    keyword_index = handle.process(get_next_audio_frame())
    if keyword_index >= 0:
        # Insert detection event callback here
        pass
```

Finally, when done be sure to explicitly release the resources using `handle.delete()`.

### .NET

Install the .NET SDK using Nuget or the dotnet CLI

```bash
dotnet add package Porcupine
```

The SDK exposes a factory method to create instances of the engine:

```csharp
using Pv

Porcupine handle = Porcupine.Create(keywords: new List<string> { "picovoice" });
```

`keywords` argument is a shorthand for accessing default keyword files shipped with the library. The default keyword
files available can be retrieved via

```csharp
using Pv

foreach (string keyword in Porcupine.KEYWORDS)
{
    Console.WriteLine(keyword);
}
```

If you wish to use a non-default keyword file you need to identify its path:

```csharp
using Pv

Porcupine handle = Porcupine.Create(
    keywordPaths: new List<string>{ "path/to/non/default/keyword/file"});
```

When initialized, valid sample rate can be obtained using `handle.SampleRate`. Expected frame length
(number of audio samples in an input array) is `handle.FrameLength`. The object can be used to monitor
incoming audio as below:

```csharp
short[] getNextAudioFrame()
{
    // .. get a frame of audio
    return audioFrame;
}

while(true)
{
    var keywordIndex = handle.Process(getNextAudioFrame())
    if(keywordIndex >= 0)
    {
        // .. Insert detection event callback here
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

### Java

Install the Porcupine Java binding by downloading and referencing the latest Porcupine JAR file available
[here](/binding/java/bin). The SDK exposes a builder to create instances of the engine:

```java
import ai.picovoice.porcupine.*;

try{
    Porcupine handle = new Porcupine.Builder()
                        .setKeyword("picovoice")
                        .build();
} catch (PorcupineException e) { }
```

The `setKeyword()` builder argument is a shorthand for accessing default keyword model files shipped with the package.  
The default keyword files available can be retrieved via:

```java
import ai.picovoice.porcupine.*;

for(String keyword : Porcupine.KEYWORDS){
    System.out.println(keyword);
}
```

If you wish to use a non-default keyword file you need to identify its path as below:

```java
import ai.picovoice.porcupine.*;

try{
    Porcupine handle = new Porcupine.Builder()
                        .setKeywordPath("path/to/non/default/keyword/file")
                        .build();
} catch (PorcupineException e) { }
```

When initialized, valid sample rate can be obtained using `handle.getSampleRate()`. Expected frame length
(number of audio samples in an input array) is `handle.getFrameLength()`. The object can be used to monitor
incoming audio as below:

```java
short[] getNextAudioFrame(){
    // .. get audioFrame
    return audioFrame;
}

while(true){
    int keywordIndex = handle.Process(getNextAudioFrame());
    if(keywordIndex >= 0){
        // .. detection event logic/callback
    }
}
```

Once you're done with Porcupine, ensure you release its resources explicitly:

```java
handle.delete();
```

### React Native

Install [@picovoice/react-native-voice-processor](https://www.npmjs.com/package/@picovoice/react-native-voice-processor)
and  [@picovoice/porcupine-react-native](https://www.npmjs.com/package/@picovoice/porcupine-react-native). The SDK
provides two APIs:

#### High-Level API

[PorcupineManager](/binding/react-native/src/porcupinemanager.tsx) provides a high-level API that takes care of 
audio recording. This class is the quickest way to get started. 

Using the constructor `PorcupineManager.fromKeywords` will create an instance of the `PorcupineManager` 
using one or more of the built-in keywords. 

```javascript
async createPorcupineManager(){
    try{
        this._porcupineManager = await PorcupineManager.fromKeywords(
            ["picovoice", "porcupine"], 
            detectionCallback);
    } catch (err) {
        // handle error
    }
}
```
NOTE: the call is asynchronous and therefore should be called in an async block with a try/catch.

To create an instance of PorcupineManager that detects custom keywords, you can use the `PorcupineManager.fromKeywordPaths` 
static constructor and provide the paths to the `.ppn` file(s).
```javascript
this._porcupineManager = await PorcupineManager.fromKeywords(
    ["/path/to/keyword.ppn"],
    detectionCallback);
```

Once you have instantiated a Porcupine manager, you can start audio capture and wake word detection by calling:

```javascript
let didStart = this._porcupineManager.start();
```

And then stop it by calling:

```javascript
let didStop = this._porcupineManager.stop();
```

Once the app is done with using PorcupineManager, be sure you explicitly release the resources allocated to Porcupine:
```javascript
this._porcupineManager.delete();
```

As you may have noticed, there is no need to deal with audio capture to enable wake word detection with PorcupineManager. 
This is because it uses [@picovoice/react-native-voice-processor](https://github.com/Picovoice/react-native-voice-processor/) 
module to capture frames of audio and automatically pass it to the wake word engine.

#### Low-Level API

[Porcupine](/binding/react-native/src/porcupine.tsx) provides low-level access to the wake word engine for those 
who want to incorporate wake word detection into a already existing audio processing pipeline.  `Porcupine` also has
`fromKeywords` and `fromKeywordPaths` static constructors.

```javascript
async createPorcupine(){
    try{
        this._porcupine = await Porcupine.fromKeywords(["picovoice"]);
    } catch (err) {
        // handle error
    }
}
```
As you can see, in this case you don't pass in a detection callback as you will be passing in audio frames directly
using the process function:

```javascript
let buffer = getAudioFrame();

try {
    let keywordIndex = await this._porcupine.process(buffer);
    if (keywordIndex >= 0) {
        // detection made!
    }
} catch (e) {
    // handle error
}
```

For process to work correctly, the audio data must be in the audio format required by Picovoice. 
The required audio format is found by calling `.sampleRate` to get the required sample rate and `.frameLength` to 
get the required frame size. Audio must be single-channel and 16-bit linearly-encoded.

Finally, once you no longer need the wake word engine, be sure to explicitly release the resources allocated to Porcupine:

```javascript
this._porcupine.delete();
```

### Android

There are two possibilities for integrating Porcupine into an Android application.

#### High-Level API

[PorcupineManager](binding/android/Porcupine/porcupine/src/main/java/ai/picovoice/porcupine/PorcupineManager.java)
provides a high-level API for integrating Porcupine into Android applications. It manages all activities related to creating
an input audio stream, feeding it into the Porcupine library, and invoking a user-provided detection callback.

```java
final String modelPath = ... // Available at lib/common/porcupine_params.pv
final String keywordPath = ...
final float sensitivity = 0.5f;

PorcupineManager manager = new PorcupineManager(
        modelPath,
        keywordPath,
        sensitivity,
        new PorcupineManagerCallback() {
            @Override
            public void invoke(int keywordIndex) {
                // detection event logic/callback
            }
        });
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

When initialized, input audio can be monitored using `manager.start()`. Stop the manager using by invoking
`manager.stop()`. When done be sure to release the resources using `manager.delete()`.

#### Low-Level API

[Porcupine](/binding/android/Porcupine/porcupine/src/main/java/ai/picovoice/porcupine/Porcupine.java) provides a
binding for Android. It can be initialized using.

```java
final String modelPath = ... // Available at lib/common/porcupine_params.pv
final String keywordPath = ...
final float sensitivity = 0.5f;

Porcupine porcupine = new Porcupine(modelPath, keywordPath, sensitivity);
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

Once initialized, `porcupine` can be used to monitor incoming audio.

```java
private short[] getNextAudioFrame();

while (true) {
    final int keywordIndex = porcupine.process(getNextAudioFrame());
    if (keywordIndex != -1) {
        // detection event logic/callback
    }
}
```

Finally, be sure to explicitly release resources acquired by porcupine as the binding class does not rely on the
garbage collector for releasing native resources.

```java
porcupine.delete();
```

### iOS

There are two approaches for integrating Porcupine into an iOS application.

#### High-Level API

[PorcupineManager](/binding/ios/PorcupineManager.swift) manages audio recording, feeding it into Porcupine, and invoking
the user-provided detection callback.

```swift
let modelPath: String = ... // Available at lib/common/porcupine_params.pv
let keywordPaths: [String] = ["/path/to/keyword/file/a", "/path/to/keyword/file/b"]
let sensitivities: [Float32] = [0.35, 0.64]
let keywordCallback: ((Int32) -> Void) = { keywordIndex in
    // Insert detection event logic
}

let manager = try PorcupineManager(
    modelPath: modelPath,
    keywordPaths: keywordPaths,
    sensitivities: sensitivities
    onDetection: keywordCallback)
```

When initialized, input audio can be monitored using `manager.start()`. When done be sure to stop the manager using
`manager.stop()`.

#### Direct

Porcupine is shipped as a precompiled ANSI C library and can directly be used in Swift using module maps. It can be
initialized to detect multiple wake words concurrently using:

```swift
let modelPath: String = ... // Available at lib/common/porcupine_params.pv
let keywordPaths: [String] = ["/path/to/keyword/file/a", "/path/to/keyword/file/b"]
let sensitivities: [Float32] = [0.35, 0.64]

var handle: OpaquePointer?
let status = pv_porcupine_init(
    modelPath,
    Int32(keywordFilePaths.count),
    keywordPaths.map{ UnsafePointer(strdup($0)) },
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

    let status = pv_porcupine_process(handle, pcm, &keyword_index)
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

### JavaScript

Porcupine is available on modern web browsers via [WebAssembly](https://webassembly.org/). The
[JavaScript binding](/binding/javascript/porcupine.js) makes it easy to use Porcupine within a browser environment.

Instantiate a new instance of engine using the factory method:

```javascript
let keywordModels = [new Uint8Array([...]), ...];
let sensitivities = new Float32Array([0.5, ...]);

let handle = Porcupine.create(keywordModels, sensitivities)
```

When instantiated, `handle` can process audio via its `.process` method.

```javascript
let getNextAudioFrame = function() {
    ...
};

while (true) {
    let keywordIndex = handle.process(getNextAudioFrame());
    if (keywordIndex !== -1) {
        // Insert detection callback
    }
}
```

When done be sure to release resources acquired by WebAssembly using `release()`:

```javascript
handle.release();
```

### NodeJS

Install NodeJS SDK:

```bash
yarn add @picovoice/porcupine-node
```

Create instances of the Porcupine class by specifying which keywords you want it to listen for:

```javascript
const Porcupine = require("@picovoice/porcupine-node");

const {
  GRASSHOPPER,
  BUMBLEBEE,
} = require("@picovoice/porcupine-node/builtin_keywords");

let handle = new Porcupine([GRASSHOPPER, BUMBLEBEE], [0.5, 0.65]);
```

`GRASSHOPPER` and `BUMBLEBEE` are builtin keywords. If you wish to use a non-default keyword file you need to identify its path:

```javascript
const Porcupine = require("@picovoice/porcupine-node");

let handle = new Porcupine(["/path/to/non/default/keyword/file"], [0.5]);
```

When instantiated, `handle` can process audio via its `.process` method.

```javascript
let getNextAudioFrame = function() {
    ...
};

while (true) {
  let keywordIndex = handle.process(getNextAudioFrame());
  if (keywordIndex !== -1) {
    // detection event callback
  }
}
```

When done be sure to release resources acquired by WebAssembly using `release()`:

```javascript
handle.release();
```

### C

Porcupine is implemented in ANSI C and therefore can be directly linked to C applications.
[include/pv_porcupine.h](/include/pv_porcupine.h) header file contains relevant information. An instance of Porcupine
object can be constructed as follows.

```c
const char *model_path = ... // Available at lib/common/porcupine_params.pv
const char *keyword_path = ...
const float sensitivity = 0.5f;

pv_porcupine_t *handle = NULL;
const pv_status_t status = pv_porcupine_init(
    model_path,
    1,
    &keyword_path,
    &sensitivity,
    &handle);
if (status != PV_STATUS_SUCCESS) {
    // Insert error handling logic
}
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating-point number
within [0, 1]. A higher sensitivity reduces miss rate (false reject rate) at cost of (potentially) increased false alarm
rate.

Now the `handle` can be used to monitor incoming audio stream. Porcupine accepts single channel, 16-bit linearly-encoded
PCM audio. The sample rate can be retrieved using `pv_sample_rate()`. Finally, Porcupine accepts input audio in
consecutive chunks (aka frames) the length of each frame can be retrieved using `pv_porcupine_frame_length()`.

```c
extern const int16_t *get_next_audio_frame(void);

while (true) {
    const int16_t *pcm = get_next_audio_frame();
    int32_t keyword_index = -1;
    const pv_status_t status = pv_porcupine_process(handle, pcm, &keyword_index);
    if (status != PV_STATUS_SUCCESS) {
        // error handling logic
    }
    if (keyword_index != -1) {
        // Insert detection event callback
    }
}
```

Finally, when done be sure to release the acquired resources.

```c
pv_porcupine_delete(handle);
```

## Releases

### v1.9.0 - December 2nd, 2020

- Added *Alexa*, *Computer*, *Hey Google*, *Hey Siri*, *Jarvis*, and *Okay Google* models under Apache 2.0.
- Added React Native SDK.
- Added Java SDK.
- Added .NET SDK.
- Added NodeJS SDK.
- Improved accuracy.
- Runtime optimization.

### v1.8.0 - May 27th, 2020

- Improved accuracy.
- Runtime optimization.

### v1.7.0 - Feb 13th, 2020

- Improved accuracy.
- Runtime optimization.
- Added support for Raspberry Pi 4.
- Added service-based Android demo application.
- Added C demo applications.
- Updated documentation.

### v1.6.0 - April 25th, 2019

- Improved accuracy.
- Runtime optimization.
- Added support for BeagleBone.
- iOS build can run on simulator now.

### v1.5.0 - November 13, 2018

- Improved accuracy.
- Runtime optimization.
- Added support for running within web browsers (WebAssembly).

### v1.4.0 - July 20, 2018

- Improved accuracy.
- Runtime optimizations.
- Updated documentation.

### v1.3.0 - June 19, 2018

- Improved accuracy.
- Runtime optimizations

### v1.2.0 - April 21, 2018

- Runtime optimizations.

### v1.1.0 - April 11, 2018

- Added multiple command detection capability.

### v1.0.0 - March 13, 2018

- Initial release.

## FAQ

You can find the FAQ [here](https://picovoice.ai/docs/faq/porcupine/).
