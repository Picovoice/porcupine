# Porcupine

[![GitHub release](https://img.shields.io/github/release/Picovoice/Porcupine.svg)](https://github.com/Picovoice/Porcupine/releases)
[![GitHub](https://img.shields.io/github/license/Picovoice/porcupine)](https://github.com/Picovoice/porcupine/)

[![Maven Central](https://img.shields.io/maven-central/v/ai.picovoice/porcupine-android?label=maven-central%20%5Bandroid%5D)](https://repo1.maven.org/maven2/ai/picovoice/porcupine-android/)
[![Maven Central](https://img.shields.io/maven-central/v/ai.picovoice/porcupine-java?label=maven%20central%20%5Bjava%5D)](https://repo1.maven.org/maven2/ai/picovoice/porcupine-java/)
[![npm](https://img.shields.io/npm/v/@picovoice/porcupine-node?label=npm%20%5Bnode%5D)](https://www.npmjs.com/package/@picovoice/porcupine-node)
[![npm](https://img.shields.io/npm/v/@picovoice/porcupine-react?label=npm%20%5Breact%5D)](https://www.npmjs.com/package/@picovoice/porcupine-react)
[![npm](https://img.shields.io/npm/v/@picovoice/porcupine-react-native?label=npm%20%5Breact-native%5D)](https://www.npmjs.com/package/@picovoice/porcupine-react-native)
[![npm](https://img.shields.io/npm/v/@picovoice/porcupine-web?label=npm%20%5Bweb%5D)](https://www.npmjs.com/package/@picovoice/porcupine-web)
[![Nuget](https://img.shields.io/nuget/v/porcupine)](https://www.nuget.org/packages/Porcupine/)
[![CocoaPods](https://img.shields.io/cocoapods/v/Porcupine-iOS)](https://cocoapods.org/pods/Porcupine-iOS)<!-- markdown-link-check-disable-line -->
[![Pub Version](https://img.shields.io/pub/v/porcupine_flutter)](https://pub.dev/packages/porcupine_flutter)
[![PyPI](https://img.shields.io/pypi/v/pvporcupine)](https://pypi.org/project/pvporcupine/)

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

[![Twitter URL](https://img.shields.io/twitter/url?label=%40AiPicovoice&style=social&url=https%3A%2F%2Ftwitter.com%2FAiPicovoice)](https://twitter.com/AiPicovoice)<!-- markdown-link-check-disable-line -->
[![YouTube Channel Views](https://img.shields.io/youtube/channel/views/UCAdi9sTCXLosG1XeqDwLx7w?label=YouTube&style=social)](https://www.youtube.com/channel/UCAdi9sTCXLosG1XeqDwLx7w)

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient. It is perfect for IoT.
- cross-platform:
  - Arm Cortex-M, STM32, and Arduino
  - Raspberry Pi (Zero, 3, 4, 5)
  - Android and iOS
  - Chrome, Safari, Firefox, and Edge
  - Linux (x86_64), macOS (x86_64, arm64), and Windows (x86_64, arm64)
- scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
- self-service. Developers can train custom wake word models using [Picovoice Console](https://console.picovoice.ai/).

## Table of Contents

- [Porcupine](#porcupine)
  - [Table of Contents](#table-of-contents)
  - [Use Cases](#use-cases)
  - [Try It Out](#try-it-out)
  - [Language Support](#language-support)
  - [Performance](#performance)
  - [Demos](#demos)
    - [Python](#python-demos)
    - [.NET](#net-demos)
    - [Java](#java-demos)
    - [Unity](#unity-demos)
    - [Flutter](#flutter-demos)
    - [React Native](#react-native-demos)
    - [Android](#android-demos)
    - [iOS](#ios-demos)
    - [Web](#web-demos)
      - [Vanilla JavaScript and HTML](#vanilla-javascript-and-html)
      - [React](#react-demos)
    - [NodeJS](#nodejs-demos)
    - [C](#c-demos)
    - [Microcontroller](#microcontroller-demos)
  - [SDKs](#sdks)
    - [Python](#python)
    - [.NET](#net)
    - [Java](#java)
    - [Unity](#unity)
    - [Flutter](#flutter)
    - [React Native](#react-native)
    - [Android](#android)
    - [iOS](#ios)
    - [Web](#web)
      - [Vanilla JavaScript and HTML (CDN Script Tag)](#vanilla-javascript-and-html-cdn-script-tag)
      - [Vanilla JavaScript and HTML (ES Modules)](#vanilla-javascript-and-html-es-modules)
      - [React](#react)
    - [NodeJS](#nodejs)
    - [C](#c)
    - [Microcontroller](#microcontroller)
  - [Releases](#releases)
  - [FAQ](#faq)

## Use Cases

Porcupine is the right product if you need to detect one or a few static (always-listening) voice commands.

- If you want to create voice experiences similar to Alexa or Google, see the
  [Picovoice platform](https://github.com/Picovoice/picovoice).
- If you need to understand complex and naturally-spoken voice commands within a specific domain, see the
  [Rhino Speech-to-Intent engine](https://github.com/Picovoice/rhino).

## Try It Out

- [Interactive Web Demo](https://picovoice.ai/demos/lamp/)

- Porcupine on a Raspberry Pi Zero

[![Porcupine in Action](https://img.youtube.com/vi/Fi_IJEcNr3I/0.jpg)](https://www.youtube.com/watch?v=Fi_IJEcNr3I)

## Language Support

- English, Chinese (Mandarin), French, German, Italian, Japanese, Korean, Portuguese, and Spanish.
- Support for additional languages is available for commercial customers on a case-by-case basis.

## Performance

A comparison between accuracy and runtime metrics of Porcupine and two other widely-used libraries, PocketSphinx and
Snowboy, is provided [here](https://github.com/Picovoice/wakeword-benchmark). Compared to the best-performing engine of
these two, Porcupine is **11.0 times more accurate** and **6.5 times faster** (on Raspberry Pi 3).

## Demos

If using SSH, clone the repository with:

```console
git clone --recurse-submodules git@github.com:Picovoice/porcupine.git
```

If using HTTPS, clone the repository with:

```console
git clone --recurse-submodules https://github.com/Picovoice/porcupine.git
```

### Python Demos

Install the demo package:

```console
sudo pip3 install pvporcupinedemo
```

With a working microphone connected to your device run the following in the terminal:

```console
porcupine_demo_mic --access_key ${ACCESS_KEY} --keywords porcupine
```

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of `Porcupine`.

For more information about Python demos go to [demo/python](demo/python).

### .NET Demos

From [demo/dotnet/PorcupineDemo](demo/dotnet/PorcupineDemo) run the
following in the terminal to build the demo:

```console
dotnet build -c MicDemo.Release
```

Make sure there is a working microphone connected to your device. From [demo/dotnet/PorcupineDemo](demo/dotnet/PorcupineDemo) run the
following in the terminal:

```console
dotnet run -c MicDemo.Release -- \
--access_key ${ACCESS_KEY} \
--keywords porcupine
```

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of `Porcupine`.

For more information about .NET demos go to [demo/dotnet](demo/dotnet).

### Java Demos

Make sure there is a working microphone connected to your device. Then invoke the following commands from the terminal:

```console
cd demo/java
./gradlew build
cd build/libs
java -jar porcupine-mic-demo.jar -a ${ACCESS_KEY} -k porcupine
```

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of `Porcupine`.

For more information about Java demos go to [demo/java](demo/java).

### Unity Demos

> Unity SDKs will no longer be maintained after **December 15, 2025**. If you plan to use the Porcupine Wake Word Unity SDK for commercial purposes, please [contact us](https://picovoice.ai/contact/).

To run the Porcupine Unity demo, import the [Porcupine Unity package](binding/unity/porcupine-3.0.1.unitypackage) into your project, open the PorcupineDemo scene and hit play. To run on other platforms or in the player, go to _File > Build Settings_, choose your platform and hit the `Build and Run` button.

To browse the demo source go to [demo/unity](demo/unity).

### Flutter Demos

To run the Porcupine demo on Android or iOS with Flutter, you must have the [Flutter SDK](https://flutter.dev/docs/get-started/install) installed on your system. Once installed, you can run `flutter doctor` to determine any other missing requirements for your relevant platform. Once your environment has been set up, launch a simulator or connect an Android/iOS device.

Run the `prepare_demo` script from [demo/flutter](./demo/flutter) with a language code to set up the demo in the language of your
choice (e.g. `de` -> German, `ko` -> Korean). To see a list of available languages, run `prepare_demo` without a language code.

```console
dart scripts/prepare_demo.dart ${LANGUAGE}
```

Replace your `AccessKey` in [lib/main.dart](./demo/flutter/lib/main.dart) file:

```dart
final String accessKey = "{YOUR_ACCESS_KEY_HERE}"; // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
```

Run the following command from [demo/flutter](demo/flutter) to build and deploy the demo to your device:

```console
flutter run
```

### React Native Demos

To run the React Native Porcupine demo app you will first need to set up your React Native environment. For this,
please refer to [React Native's documentation](https://reactnative.dev/docs/environment-setup).

Replace your `AccessKey`, in [`App.tsx`](./demo/react-native/App.tsx) file:
```typescript
_accessKey: string ="${YOUR_ACCESS_KEY_HERE}" // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
```

Navigate to [demo/react-native](demo/react-native) to run the following commands:

For Android:

```console
yarn android-install          # sets up environment
yarn android-run ${LANGUAGE}  # builds and deploys to Android
```

For iOS:

```console
yarn ios-install              # sets up environment
yarn ios-run ${LANGUAGE}      # builds and deploys to iOS
```

Replace `${LANGUAGE}` with the language code of your choice (e.g. de -> German, ko -> Korean).
To see a list of available languages, run the `android-run` or `ios-run` command without a language code.

### Android Demos

Using [Android Studio](https://developer.android.com/studio/index.html), open
[demo/android/Activity](demo/android/Activity) as an Android project, copy your AccessKey into `MainActivity.java`, select the build variant (`Build > Select Build Variant...`) for the desired language and then run the application.

To learn about how to use Porcupine in long-running services go to [demo/android/Service](demo/android/Service).

To learn about how to use Porcupine with Android Speech to Text recognition go to [demo/android/STT](demo/android/STT).

### iOS Demos

The BackgroundService demo runs audio recording in the background, enabling detection of wake word while the application is **not** in focus and remains running in the background.
The ForegroundApp demo runs wake word detection **only** when the application is in focus.

#### BackgroundService Demo

1) To run the demo, go to [demo/ios/BackgroundService](demo/ios/BackgroundService).

2) Open the `BackgroundService.xcodeproj` in XCode

3) Replace `let accessKey = "${YOUR_ACCESS_KEY_HERE}"` in the file [ViewModel.swift](./demo/ios/BackgroundService/BackgroundService/ViewModel.swift) with your `AccessKey`.

4) Run the demo with a simulator or connected iOS device

#### ForegroundApp Demo

To run the foreground application demo:

1) Go to [demo/ios/ForegroundApp](./demo/ios/ForegroundApp) directory.

2) Open the `ForegroundApp.xcodeproj` in XCode

3) Replace `let accessKey = "${YOUR_ACCESS_KEY_HERE}"` in the file [ViewModel.swift](./demo/ios/ForegroundApp/ForegroundApp/ViewModel.swift) with your `AccessKey`.

4) Go to `Product > Scheme` and select the scheme for the language you would like to demo (e.g. `esDemo` -> Spanish Demo, `deDemo` -> German Demo)

5) Run the demo with a simulator or connected iOS device

The demo allows you to select any of the pre-built keywords for detection. Press start and say the selected keyword.


### Web Demos

#### Vanilla JavaScript and HTML

From [demo/web](demo/web) run the following in the terminal:

```console
yarn
yarn start ${LANGUAGE}
```

(or)

```console
npm install
npm run start ${LANGUAGE}
```

Open `http://localhost:5000` in your browser to try the demo.

#### React Demos

From [demo/react](demo/react) run the following in the terminal:

```console
yarn
yarn start ${LANGUAGE}
```

(or)

```console
npm install
npm run start ${LANGUAGE}
```

Open `http://localhost:3000` in your browser to try the demo.

### NodeJS Demos

Install the demo package:

```console
yarn global add @picovoice/porcupine-node-demo
```

With a working microphone connected to your device run the following in the terminal:

```console
ppn-mic-demo --access_key ${ACCESS_KEY} --keywords porcupine
```

The engine starts processing the audio input from the microphone in realtime and outputs to the terminal when it detects
utterances of `Porcupine`.

For more information about NodeJS demos go to [demo/nodejs](demo/nodejs).

### C Demos

The C demo requires [CMake](https://cmake.org/) version 3.4 or higher.

The [Microphone demo](demo/c/porcupine_demo_mic.c) requires  [miniaudio](https://github.com/mackron/miniaudio) for accessing microphone audio data.

**Windows Requires [MinGW](http://mingw-w64.org) to build the demo.**

#### Microphone Demo

At the root of the repository, build with:

```console
cmake -S demo/c/. -B demo/c/build && cmake --build demo/c/build --target porcupine_demo_mic
```

#### Linux (x86_64), macOS (x86_64, arm64), and Raspberry Pi (Zero, 3, 4, 5)

List input audio devices with:

```console
./demo/c/build/porcupine_demo_mic --show_audio_devices
```

Run the demo using:

```console
./demo/c/build/porcupine_demo_mic -l ${LIBRARY_PATH} -m lib/common/porcupine_params.pv \
-k resources/keyword_files/${PLATFORM}/porcupine_${PLATFORM}.ppn -t 0.5 \
-d ${AUDIO_DEVICE_INDEX} -a ${ACCESS_KEY}
```

Replace `${LIBRARY_PATH}` with path to appropriate library available under [lib](lib), `${PLATFORM}` with the
name of the platform you are running on (`linux`, `raspberry-pi`, or `mac`), `${AUDIO_DEVICE_INDEX}` with
the index of your audio device and `${ACCESS_KEY}` with your `AccessKey`.

#### Windows

List input audio devices with:

```console
.\\demo\\c\\build\\porcupine_demo_mic.exe --show_audio_devices
```

Run the demo using:

```console
.\\demo\\c\\build\\porcupine_demo_mic.exe ^
-l lib/windows/amd64/libpv_porcupine.dll ^
-m lib/common/porcupine_params.pv ^
-k resources/keyword_files/windows/porcupine_windows.ppn ^
-t 0.5 ^
-d ${AUDIO_DEVICE_INDEX} ^
-a ${ACCESS_KEY}
```

Replace `${AUDIO_DEVICE_INDEX}` with the index of your audio device and `${ACCESS_KEY}` with your `AccessKey`.

The demo opens an audio stream and detects utterances of `Porcupine`.

#### File Demo

At the root of the repository, build with:

```console
cmake -S demo/c/. -B demo/c/build && cmake --build demo/c/build --target porcupine_demo_file
```

#### Linux (x86_64), macOS (x86_64, arm64), and Raspberry Pi (Zero, 3, 4, 5)

Run the demo using:

```console
./demo/c/build/porcupine_demo_file -l ${LIBRARY_PATH} -m lib/common/porcupine_params.pv \
-k resources/keyword_files/${PLATFORM}/porcupine_${PLATFORM}.ppn -t 0.5 \
-w resources/audio_samples/multiple_keywords.wav -a ${ACCESS_KEY}
```

Replace `${LIBRARY_PATH}` with path to appropriate library available under [lib](lib), `${PLATFORM}` with the
name of the platform you are running on (`linux`, `raspberry-pi`, or `mac`) and `${ACCESS_KEY}` with your `AccessKey`.

#### Windows

Run the demo using:

```console
.\\demo\\c\\build\\porcupine_demo_file.exe ^
-l lib/windows/amd64/libpv_porcupine.dll ^
-m lib/common/porcupine_params.pv ^
-k resources/keyword_files/windows/porcupine_windows.ppn ^
-t 0.5 ^
-w resources/audio_samples/multiple_keywords.wav ^
-a ${ACCESS_KEY}
```

Replace `${ACCESS_KEY}` with your `AccessKey`.

The demo opens up the file and detects utterances of `Porcupine`.

For more information about C demos go to [demo/c](demo/c).

### Microcontroller Demos

There are several projects for various development boards inside the [mcu demo](demo/mcu) folder.

## SDKs

### Python

Install the Python SDK:

```console
pip3 install pvporcupine
```

The SDK exposes a factory method to create instances of the engine:

```python
import pvporcupine

# AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
access_key = "${ACCESS_KEY}"

handle = pvporcupine.create(access_key=access_key, keywords=['picovoice', 'bumblebee'])
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

# AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
access_key = "${ACCESS_KEY}"

handle = pvporcupine.create(
    access_key=access_key,
    keyword_paths=['path/to/non/default/keyword/file'])
```

When initialized, valid sample rate can be obtained using `handle.sample_rate`. The required frame length
(number of audio samples in an input array) is `handle.frame_length`. The object can be used to monitor
incoming audio as follows:

```python
import pvporcupine

# AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
access_key = "${ACCESS_KEY}"

handle = pvporcupine.create(access_key=access_key, keywords=['porcupine'])

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

Install the .NET SDK using NuGet or the dotnet CLI:

```console
dotnet add package Porcupine
```

The SDK exposes a factory method to create instances of the engine:

```csharp
using Pv;

const string accessKey = "${ACCESS_KEY}";
var keyword = new List<BuiltInKeyword> { BuiltInKeyword.PICOVOICE };

Porcupine handle = Porcupine.FromBuiltInKeywords(accessKey, keyword);
```

Using the `FromBuiltInKeywords` constructor allows you to initialize the Porcupine engine to detect any of the free, built-in keywords that come with the library. These built-ins are represented by the `BuiltInKeyword` enum.

If you wish to use a custom keyword file (i.e. a keyword file generated by Picovoice Console, with a `.ppn` extension), you need to specify its path:

```csharp
const string accessKey = "${ACCESS_KEY}";
var keywordPaths = new List<string> {
    "/absolute/path/to/keyword/one",
    "/absolute/path/to/keyword/two",
    ... }

Porcupine handle = Porcupine.FromKeywordPaths(accessKey, keywordPaths);
```

When initialized, the required sample rate can be obtained using `handle.SampleRate`. Expected frame length
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

Porcupine will have its resources freed by the garbage collector, but to have resources freed immediately after use,
wrap it in a `using` statement:

```csharp
using(Porcupine handle = Porcupine.FromBuiltInKeywords(
    accessKey,
    new List<BuiltInKeyword> { BuiltInKeyword.PICOVOICE }))
{
    // .. Porcupine usage here
}
```

### Java

The Porcupine Java binding is available from the Maven Central Repository at `ai.picovoice:porcupine-java:${version}`.


```java
import ai.picovoice.porcupine.*;

// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
final String accessKey = "${ACCESS_KEY}";
try {
    Porcupine handle = new Porcupine.Builder()
                        .setAccessKey(accessKey)
                        .setBuiltInKeyword(BuiltInKeyword.PORCUPINE)
                        .build();
} catch (PorcupineException e) { }
```

The `setBuiltInKeyword()` builder argument is a shorthand for accessing built-in keyword model files shipped with the package.

The list of built-in keywords can be found in the `BuiltInKeyword` enum, and can be retrieved by:

```java
import ai.picovoice.porcupine.*;

for(BuiltInKeyword keyword : BuiltInKeyword.values()) {
    System.out.println(keyword.name());
}
```

If you wish to use a custom keyword file (i.e. a keyword file generated by Picovoice Console, with a `.ppn` extension) you need to the file path as demonstrated below:

```java
import ai.picovoice.porcupine.*;

// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
final String accessKey = "${ACCESS_KEY}";
try {
    Porcupine handle = new Porcupine.Builder()
                        .setAccessKey(accessKey)
                        .setKeywordPath("path/to/custom/keyword/file")
                        .build();
} catch (PorcupineException e) { }
```

When initialized, valid sample rate can be obtained using `handle.getSampleRate()`. Expected frame length
(number of audio samples in an input array) is `handle.getFrameLength()`. The object can be used to monitor
incoming audio as below:

```java
short[] getNextAudioFrame() {
    // .. get audioFrame
    return audioFrame;
}

while(true) {
    int keywordIndex = handle.Process(getNextAudioFrame());
    if(keywordIndex >= 0) {
        // .. detection event logic/callback
    }
}
```

Once you're done with Porcupine, ensure you release its resources explicitly:

```java
handle.delete();
```

### Unity

> Unity SDKs will no longer be maintained after **December 15, 2025**. If you plan to use the Porcupine Wake Word Unity SDK for commercial purposes, please [contact us](https://picovoice.ai/contact/).

Import the [Porcupine Unity Package](binding/unity/porcupine-3.0.1.unitypackage) into your Unity project.

The SDK provides two APIs:

#### High-Level API

[PorcupineManager](binding/unity/Assets/Porcupine/PorcupineManager.cs) provides a high-level API that takes care of audio recording. This is the quickest way to get started.

The static constructor `PorcupineManager.FromBuiltInKeywords` will create an instance of the `PorcupineManager` using one or more of the built-in keywords.

```csharp
using Pv.Unity;

// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
string accessKey = "${ACCESS_KEY}";

try {
    List<Porcupine.BuiltInKeyword> keywords = new List<Porcupine.BuiltInKeyword>(){
        Porcupine.BuiltInKeyword.PICOVOICE,
        Porcupine.BuiltInKeyword.PORCUPINE
    };
    PorcupineManager _porcupineManager = PorcupineManager.FromBuiltInKeywords(
                                            accessKey,
                                            keywords,
                                            OnWakeWordDetected);
}
catch (Exception ex)
{
    // handle porcupine init error
}
```

To create an instance of PorcupineManager that detects custom keywords, you can use the `PorcupineManager.FromKeywordPaths`
static constructor and provide the paths to the `.ppn` file(s).

```csharp
// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
string accessKey = "${ACCESS_KEY}";

List<string> keywordPaths = new List<string>(){ "/path/to/keyword.ppn" };
PorcupineManager _porcupineManager = PorcupineManager.FromKeywordPaths(
                                        accessKey,
                                        keywordPaths,
                                        OnWakeWordDetected);
```

Once you have instantiated a PorcupineManager, you can start/stop audio capture and wake word detection by calling:

```csharp
_porcupineManager.Start();
// .. use porcupine
_porcupineManager.Stop();
```

Once the app is done with using PorcupineManager, you can explicitly release the resources allocated to Porcupine:

```csharp
_porcupineManager.Delete();
```

There is no need to deal with audio capture to enable wake word detection with PorcupineManager.
This is because it uses our
[unity-voice-processor](https://github.com/Picovoice/unity-voice-processor/)
Unity package to capture frames of audio and automatically pass it to the wake word engine.

#### Low-Level API

[Porcupine](binding/unity/Assets/Porcupine/Porcupine.cs) provides low-level access to the wake word engine for those who want to incorporate wake word detection into an already existing audio processing pipeline. To create an instance of `Porcupine`, use the `.FromBuiltInKeywords` static constructor.

```csharp
using Pv.Unity;

// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
string accessKey = "${ACCESS_KEY}";

try
{
    List<Porcupine.BuiltInKeyword> keywords = new List<Porcupine.BuiltInKeyword>(){
        Porcupine.BuiltInKeyword.PORCUPINE,
        Porcupine.BuiltInKeyword.PICOVOICE
    };
    Porcupine _porcupine = Porcupine.FromBuiltInKeywords(
        accessKey: accessKey,
        keywords: keywords);
}
catch (Exception ex)
{
    // handle porcupine init error
}
```

To search for a keyword in audio, you must pass frames of audio to Porcupine using the `Process` function. The `keywordIndex` returned will either be -1 if no detection was made or an integer specifying which keyword was detected.

```csharp
short[] frame = getAudioFrame();

try
{
    int keywordIndex = _porcupine.Process(frame);
    if (keywordIndex >= 0)
    {
        // detection made!
    }
}
catch (Exception ex)
{
    Debug.LogError(ex.ToString());
}
```

For `Process` to work correctly, the provided audio must be single-channel and 16-bit linearly-encoded.

Finally, once you no longer need the wake word engine, you can explicitly release the resources allocated to Porcupine:

```csharp
_porcupine.Dispose();
```

### Flutter

Add the [Porcupine Flutter plugin](https://pub.dev/packages/porcupine) to your pub.yaml.

```yaml
dependencies:
  flutter_porcupine: ^<version>
```

The SDK provides two APIs:

#### High-Level API

[PorcupineManager](binding/flutter/lib/porcupine_manager.dart) provides a high-level API that takes care of audio recording. This class is the quickest way to get started.

The static constructor `PorcupineManager.fromBuiltInKeywords` will create an instance of the `PorcupineManager` using one or more of the built-in keywords.

```dart
import 'package:porcupine_flutter/porcupine_manager.dart';
import 'package:porcupine_flutter/porcupine_error.dart';

// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
final String accessKey = "{ACCESS_KEY}";

void createPorcupineManager() async {
    try {
        _porcupineManager = await PorcupineManager.fromBuiltInKeywords(
            accessKey,
            [BuiltInKeyword.PICOVOICE, BuiltInKeyword.PORCUPINE],
            _wakeWordCallback);
    } on PorcupineException catch (err) {
        // handle porcupine init error
    }
}
```

To create an instance of PorcupineManager that detects custom keywords, you can use the `PorcupineManager.fromKeywordPaths` static constructor and provide the paths to the `.ppn` file(s).

```dart
 // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
final String accessKey = "{ACCESS_KEY}";

_porcupineManager = await PorcupineManager.fromKeywordPaths(
    accessKey,
    ["/path/to/keyword.ppn"],
    _wakeWordCallback);
```

Once you have instantiated a PorcupineManager, you can start/stop audio capture and wake word detection by calling:

```dart
try {
    await _porcupineManager.start();
} on PorcupineException catch (ex) {
    // deal with either audio exception
}
// .. use porcupine
await _porcupineManager.stop();
```

Once the app is done with using PorcupineManager, be sure you explicitly release the resources allocated to Porcupine:

```dart
await _porcupineManager.delete();
```

There is no need to deal with audio capture to enable wake word detection with PorcupineManager.
This is because it uses [flutter_voice_processor](https://github.com/Picovoice/flutter-voice-processor/) plugin to capture frames of audio and automatically pass it to the wake word engine.

#### Low-Level API

[Porcupine](binding/flutter/lib/porcupine.dart) provides low-level access to the wake word engine for those who want to incorporate wake word detection into an already existing audio processing pipeline.`Porcupine` has `fromBuiltInKeywords` and `fromKeywordPaths` static constructors.

```dart
import 'package:porcupine_flutter/porcupine_manager.dart';
import 'package:porcupine_flutter/porcupine_error.dart';

// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
final String accessKey = "{ACCESS_KEY}";

void createPorcupine() async {
    try {
        _porcupine = await Porcupine.fromBuiltInKeywords(
          accessKey,
          [BuiltInKeyword.PICOVOICE]);
    } on PorcupineException catch (err) {
        // handle porcupine init error
    }
}
```

To search for a keyword in audio, you must pass frames of audio to Porcupine using the `process` function. The `keywordIndex` returned will either be -1 if no detection was made or an integer specifying which keyword was detected.

```dart
List<int> buffer = getAudioFrame();

try {
    int keywordIndex = _porcupine.process(buffer);
    if (keywordIndex >= 0) {
        // detection made!
    }
} on PorcupineException catch (error) {
    // handle error
}
```

For `process` to work correctly, the provided audio must be single-channel and 16-bit linearly-encoded.

Finally, once you no longer need the wake word engine, be sure to explicitly release the resources allocated to Porcupine:

```dart
_porcupine.delete();
```

### React Native

Install [@picovoice/react-native-voice-processor](https://www.npmjs.com/package/@picovoice/react-native-voice-processor)
and [@picovoice/porcupine-react-native](https://www.npmjs.com/package/@picovoice/porcupine-react-native). The SDK
provides two APIs:

#### High-Level API

[PorcupineManager](binding/react-native/src/porcupine_manager.tsx) provides a high-level API that takes care of
audio recording. This class is the quickest way to get started.

Using the constructor `PorcupineManager.fromBuiltInKeywords` will create an instance of the `PorcupineManager`
using one or more of the built-in keywords.

```javascript
 // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
const accessKey = "${ACCESS_KEY}";

async createPorcupineManager(){
    try {
        this._porcupineManager = await PorcupineManager.fromBuiltInKeywords(
            accessKey,
            [BuiltInKeywords.Picovoice, BuiltInKeywords.Porcupine],
            detectionCallback,
            processErrorCallback);
    } catch (err) {
        // handle error
    }
}
```

To create an instance of PorcupineManager that detects custom keywords, you can use the `PorcupineManager.fromKeywordPaths`
static constructor and provide the paths to the `.ppn` file(s).

```javascript
// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
const accessKey = "${ACCESS_KEY}";

this._porcupineManager = await PorcupineManager.fromKeywordPaths(
  accessKey,
  ["/path/to/keyword.ppn"],
  detectionCallback,
  processErrorCallback
);
```

Once you have instantiated a Porcupine manager, you can start/stop audio capture and wake word detection by calling:

```javascript
let didStart = this._porcupineManager.start();
// .. use Porcupine
let didStop = this._porcupineManager.stop();
```

Once the app is done with using PorcupineManager, be sure you explicitly release the resources allocated to Porcupine:

```javascript
this._porcupineManager.delete();
```

There is no need to deal with audio capture to enable wake word detection with PorcupineManager.
This is because it uses [@picovoice/react-native-voice-processor](https://github.com/Picovoice/react-native-voice-processor/)
module to capture frames of audio and automatically pass it to the wake word engine.

#### Low-Level API

[Porcupine](binding/react-native/src/porcupine.tsx) provides low-level access to the wake word engine for those
who want to incorporate wake word detection into an already existing audio processing pipeline. `Porcupine` also has
`fromBuiltInKeywords` and `fromKeywordPaths` static constructors.

```javascript
// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
const accessKey = "${ACCESS_KEY}";

async createPorcupine(){
    try {
        this._porcupine = await Porcupine.fromBuiltInKeywords(
            accessKey, [BuiltInKeywords.PICOVOICE]);
    } catch (err) {
        // handle error
    }
}
```

To search for a keyword in audio, you must pass frames of audio to Porcupine using the `process` function. The `keywordIndex` returned will either be -1 if no detection was made or an integer specifying which keyword was detected.

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

For `process` to work correctly, the provided audio must be single-channel and 16-bit linearly-encoded.

Finally, once you no longer need the wake word engine, be sure to explicitly release the resources allocated to Porcupine:

```javascript
this._porcupine.delete();
```

### Android

To include the package in your Android project, ensure you have included `mavenCentral()` in your top-level `build.gradle` file and then add the following to your app's `build.gradle`:

```groovy
dependencies {
    implementation 'ai.picovoice:porcupine-android:${LATEST_VERSION}'
}
```

There are two possibilities for integrating Porcupine into an Android application.

#### High-Level API

[PorcupineManager](binding/android/Porcupine/porcupine/src/main/java/ai/picovoice/porcupine/PorcupineManager.java)
provides a high-level API for integrating Porcupine into Android applications. It manages all activities related to creating
an input audio stream, feeding it into the Porcupine library, and invoking a user-provided detection callback.

```java
import ai.picovoice.porcupine.*;

final String accessKey = "${ACCESS_KEY}"; // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
final String keywordPath = "/path/to/keyword.ppn"; // path relative to 'assets' folder

try {
    PorcupineManager porcupineManager = new PorcupineManager.Builder()
                        .setAccessKey(accessKey)
                        .setKeywordPath(keywordPath)
                        .setSensitivity(0.5f)
                        .build(context,
                        new PorcupineManagerCallback() {
                            @Override
                            public void invoke(int keywordIndex) {
                                // detection event logic/callback
                            }
                        });
} catch (PorcupineException e) { }
```
Keyword files (`.ppn`) should be placed under the Android project assets folder (`src/main/assets/`).

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating point number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

When initialized, input audio can be monitored using `manager.start()`. Stop the manager using by invoking
`manager.stop()`. When done be sure to release the resources using `manager.delete()`.

#### Low-Level API

[Porcupine](binding/android/Porcupine/porcupine/src/main/java/ai/picovoice/porcupine/Porcupine.java) provides a
binding for Android. It can be initialized using:

```java
import ai.picovoice.porcupine.*;

final String accessKey = "${ACCESS_KEY}";
final String keywordPath = "/path/to/keyword.ppn";
try {
    Porcupine porcupine = new Porcupine.Builder()
                        .setAccessKey(accessKey)
                        .setKeywordPath(keywordPath)
                        .setSensitivity(0.5f)
                        .build(context);
} catch (PorcupineException e) { }
```
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

Finally, be sure to explicitly release resources acquired by porcupine as the binding class does not rely on the garbage collector for releasing native resources.

```java
porcupine.delete();
```

### iOS

There are two approaches for integrating Porcupine into an iOS application.

#### High-Level API

[PorcupineManager](binding/ios/PorcupineManager.swift) manages audio recording, passing it into Porcupine, and invoking
the user-provided detection callback.

```swift
let accessKey = "${ACCESS_KEY}" // Obtained from Picovoice Console (https://console.picovoice.ai)
let modelPath: String = ... // Available at lib/common/porcupine_params.pv
let keywordPaths: [String] = ["/path/to/keyword/file/a", "/path/to/keyword/file/b"]
let sensitivities: [Float32] = [0.35, 0.64]
let keywordCallback: ((Int32) -> Void) = { keywordIndex in
    // Insert detection event logic
}

let manager = try PorcupineManager(
    accessKey: accessKey,
    modelPath: modelPath,
    keywordPaths: keywordPaths,
    sensitivities: sensitivities
    onDetection: keywordCallback)
```

When initialized, input audio can be monitored using `manager.start()`. When done be sure to stop the manager using
`manager.stop()`.

#### Low-Level API

[Porcupine.swift](binding/ios/Porcupine.swift) provides low-level access to the wake word engine for those who want to incorporate wake word detection into an already existing audio processing pipeline.

To construct an instance of Porcupine, pass it a keyword.

```swift
import Porcupine

let accessKey = "${ACCESS_KEY}" // Obtained from Picovoice Console (https://console.picovoice.ai)

do {
    Porcupine porcupine = try Porcupine(
      accessKey: accessKey,
      keyword: Porcupine.BuiltInKeyword.picovoice)
} catch { }
```

To search for a keyword in audio, you must pass frames of audio to Porcupine using the `process` function. The `keywordIndex` returned will either be -1 if no detection was made or an integer specifying which keyword was detected.

```swift
func getNextAudioFrame() -> [Int16] {
    // .. get audioFrame
    return audioFrame;
}

while true {
    do {
        let keywordIndex = try porcupine.process(getNextAudioFrame())
        if keywordIndex >= 0 {
            // .. detection made!
        }
    } catch { }
}
```

Once you're done with Porcupine you can force it to release its native resources rather than waiting for the garbage collector:
```swift
porcupine.delete();
```

### Web

Porcupine is available on modern web browsers (i.e. not Internet Explorer) via [WebAssembly](https://webassembly.org/). Microphone audio is handled via the [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API) and is abstracted by the WebVoiceProcessor, which also handles downsampling to the correct format. Porcupine is provided pre-packaged as a [Web Worker](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API/Using_web_workers).

#### Vanilla JavaScript and HTML (CDN Script Tag)

```html
<!DOCTYPE html>
<html lang="en">
<head>
  <script src="https://unpkg.com/@picovoice/porcupine-web/dist/iife/index.js"></script>
  <script src="https://unpkg.com/@picovoice/web-voice-processor/dist/iife/index.js"></script>
  <script type="application/javascript">

    const PORCUPINE_MODEL_BASE64 = /* Base64 representation of the `.pv` model file*/;

    function keywordDetectionCallback(detection) {
      console.log(`Porcupine detected ${detection.label}`);
    }

    function processErrorCallback(error) {
      console.error(error);
    }

    async function startPorcupine() {
      console.log("Porcupine is loading. Please wait...");
      const accessKey = "${ACCESS_KEY}" // Obtained from Picovoice Console (picovoice.ai/console/)
      let porcupine = await PorcupineWeb.PorcupineWorker.create(
              accessKey,
              [PorcupineWeb.BuiltInKeyword.Picovoice],
              porcupineKeywordCallback,
              { base64: PORCUPINE_MODEL_BASE64 },
      );

      console.log("Porcupine worker ready!");

      console.log("WebVoiceProcessor initializing. Microphone permissions requested ...");
      await window.WebVoiceProcessor.WebVoiceProcessor.subscribe(porcupine);
      console.log("WebVoiceProcessor ready and listening!");

      document.addEventListener("DOMContentLoaded", function () {
        startPorcupine();
      });
  </script>
</head>
<body></body>
</html>
```

#### Vanilla JavaScript and HTML (ES Modules)

Install the web SDK using yarn:

```console
yarn add @picovoice/porcupine-web @picovoice/web-voice-processor
```

(or)

```console
npm install --save @picovoice/porcupine-web @picovoice/web-voice-processor
```

```javascript
import { WebVoiceProcessor } from "@picovoice/web-voice-processor"
import { PorcupineWorker } from "@picovoice/porcupine-web";

const PORCUPINE_MODEL_BASE64 = /* Base64 representation of the `.pv` model file*/;

function keywordDetectionCallback(detection) {
  console.log(`Porcupine detected ${detection.label}`);
}

const porcupine = await PorcupineWorker.create(
  "${ACCESS_KEY}",
  PorcupineWeb.BuiltInKeyword.Porcupine,
  keywordDetectionCallback,
  { base64: PORCUPINE_MODEL_BASE64 },
);

console.log("WebVoiceProcessor initializing. Microphone permissions requested ...");
await WebVoiceProcessor.subscribe(porcupine);
console.log("WebVoiceProcessor ready and listening!");

...

// Finished with Porcupine? Release the WebVoiceProcessor and the worker.
if (done) {
  await WebVoiceProcessor.unsubscribe(porcupine);
  porcupine.release()
  porcupine.terminate()
}
```

#### React

```console
yarn add @picovoice/porcupine-react @picovoice/web-voice-processor
```

(or)

```console
npm install @picovoice/porcupine-react @picovoice/web-voice-processor
```

```javascript
import { BuiltInKeyword } from '@picovoice/porcupine-web';
import { usePorcupine } from '@picovoice/porcupine-react';

function App(props) {
  const {
    wakeWordDetection,
    isLoaded,
    isListening,
    error,
    init,
    start,
    stop,
    release,
  } = usePorcupine();

  const initEngine = async () => {
    await init(
      ${ACCESS_KEY},
      [BuiltInKeyword.Porcupine],
      porcupineModel
    );
    await start();
  }

  useEffect(() => {
    if (wakeWordDetection !== null) {
      console.log(wakeWordDetection.label);
    }
  }, [wakeWordDetection])
}
```

### NodeJS

Install NodeJS SDK:

```console
yarn add @picovoice/porcupine-node
```

Create instances of the Porcupine class by specifying which keywords you want it to listen for:

```javascript
const {
  Porcupine,
  BuiltinKeyword,
}= require("@picovoice/porcupine-node");

 // Obtained from the Picovoice Console (https://console.picovoice.ai/)
const accessKey = "${ACCESS_KEY}";

let handle = new Porcupine(
    accessKey,
    [BuiltinKeyword.GRASSHOPPER, BuiltinKeyword.BUMBLEBEE],
    [0.5, 0.65]);
```

`GRASSHOPPER` and `BUMBLEBEE` are built-in keywords. If you wish to use a custom keyword file, you need to identify its path:

```javascript
const Porcupine = require("@picovoice/porcupine-node");

 // Obtained from the Picovoice Console (https://console.picovoice.ai/)
const accessKey = "${ACCESS_KEY}";

let handle = new Porcupine(
    accessKey,
    ["/path/to/custom/keyword/file"],
    [0.5]);
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
[include/pv_porcupine.h](include/pv_porcupine.h) header file contains relevant information. An instance of Porcupine
object can be constructed as follows.

```c
// Available at lib/common/porcupine_params.pv
const char *model_path = ...
// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
const char *access_key = ...
const char *keyword_path = ...
const float sensitivity = 0.5f;

pv_porcupine_t *handle = NULL;
const pv_status_t status = pv_porcupine_init(
    access_key,
    model_path,
    1,
    &keyword_path,
    &sensitivity,
    &handle);
if (status != PV_STATUS_SUCCESS) {
    // Insert error handling logic
}
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating point number
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

Finally, when done be sure to release the acquired resources:

```c
pv_porcupine_delete(handle);
```

### Microcontroller

Porcupine is implemented in ANSI C and therefore can be directly linked to embedded C projects. Its public header file contains relevant information. An instance of the Porcupine object can be constructed as follows.

```c
#define MEMORY_BUFFER_SIZE ...
uint8_t memory_buffer[MEMORY_BUFFER_SIZE] __attribute__((aligned(16)));

// AccessKey string obtained from Picovoice Console (https://console.picovoice.ai/)
static const char* ACCESS_KEY = ...

const uint8_t keyword_array[] = {...};

const int32_t keyword_model_sizes = sizeof(keyword_array);
const void *keyword_models = keyword_array;
const float sensitivity = 0.5f;

pv_porcupine_t *handle = NULL;

const pv_status_t status = pv_porcupine_init(
        ACCESS_KEY,
        MEMORY_BUFFER_SIZE,
        memory_buffer,
        1,
        &keyword_model_sizes,
        &keyword_models,
        &sensitivity,
        &handle);

if (status != PV_STATUS_SUCCESS) {
    // error handling logic
}
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating-point number
within [0, 1]. A higher sensitivity reduces miss rate (false reject rate) at cost of increased false alarm rate.

Now the `handle` can be used to monitor incoming audio stream. Porcupine accepts single channel, 16-bit PCM audio. The
sample rate can be retrieved using `pv_sample_rate()`. Finally, Picovoice accepts input audio in consecutive chunks (aka
frames) the length of each frame can be retrieved using `pv_porcupine_frame_length()`.

```c
extern const int16_t *get_next_audio_frame(void);

while (true) {
    const int16_t *pcm = get_next_audio_frame();
    int32_t keyword_index;
    const pv_status_t status = pv_porcupine_process(handle, pcm, &keyword_index);
    if (status != PV_STATUS_SUCCESS) {
        // error handling logic
    }
    if (keyword_index != -1) {
        // detection event logic/callback
    }
}
```

Finally, when done be sure to release the acquired resources.

```c
pv_porcupine_delete(handle);
```

## Releases

### v3.0.0 - October 26th, 2023

- Improvements to error reporting
- Upgrades to authorization and authentication system
- Various bug fixes and improvements
- Added Farsi support for microcontrollers
- Node min support bumped to 16
- Unity editor min support bumped to 2021
- Patches to .NET support

### v2.2.0 - April 12th, 2023

- Added language support for Arabic, Dutch, Hindi, Mandarin, Polish, Russian, Swedish and Vietnamese
- Added support for .NET 7.0 and fixed support for .NET Standard 2.0
- iOS minimum support moved to 11.0
- Improved stability and performance

### v2.1.0 - January 20th, 2022

- Added macOS arm64 (Apple Silicon) support for Java and Unity SDKs
- Various bug fixes and improvements

### v2.0.0 - Nov 25th, 2021

- Improved accuracy
- Added Rust SDK
- macOS arm64 support
- Added NodeJS support for Windows, NVIDIA Jetson Nano, and BeagleBone
- Added .NET support for NVIDIA Jetson Nano and BeagleBone
- Runtime optimization

### v1.9.0 - December 2nd, 2020

- Added React Native SDK
- Added Java SDK
- Added .NET SDK
- Added NodeJS SDK
- Improved accuracy
- Runtime optimization

### v1.8.0 - May 27th, 2020

- Improved accuracy
- Runtime optimization

### v1.7.0 - Feb 13th, 2020

- Improved accuracy
- Runtime optimization
- Added support for Raspberry Pi 4
- Added service-based Android demo application
- Added C demo applications
- Updated documentation

### v1.6.0 - April 25th, 2019

- Improved accuracy
- Runtime optimization
- Added support for BeagleBone
- iOS build can run on simulator now

### v1.5.0 - November 13, 2018

- Improved accuracy
- Runtime optimization
- Added support for running within web browsers (WebAssembly)

### v1.4.0 - July 20, 2018

- Improved accuracy
- Runtime optimizations.
- Updated documentation

### v1.3.0 - June 19, 2018

- Improved accuracy
- Runtime optimizations

### v1.2.0 - April 21, 2018

- Runtime optimizations

### v1.1.0 - April 11, 2018

- Added multiple command detection capability

### v1.0.0 - March 13, 2018

- Initial release

## FAQ

You can find the FAQ [here](https://picovoice.ai/docs/faq/porcupine/).
