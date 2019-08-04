# Android Library (High-Level API) and Demo Application

This section contains the source code for Android demo application. The application uses a
high-level library, [PorcupineManager](/demo/android/porcupinemanager), for wake word detection. The library can be 
used on its own to build always-listening Android applications. It manages all aspects of wake word detection 
including configuring Porcupine, recording input audio, and sending notifications.

## Prerequisites

Consult the prerequisites section of [Android binding](/binding/android).

## Demo Application Usage

Using [Android Studio](https://developer.android.com/studio/index.html) open [demo/android](/demo/android) as an android
project and then run the application. Note that you need an android phone with developer options enabled connected to
your machine in order to run the application.

If you'd like to run tests navigate to [androidTest](/demo/android/app/src/androidTest/java/ai/picovoice/porcupine/demo) using Android Studio,
right click, and select `Run Tests in ai.picovoice.porcupine.demo` from drop down menu.

### Note for Building on Windows

This repository is using [symlinks](https://en.wikipedia.org/wiki/Symbolic_link). On Windows symlinks do not work as
expected. You need to copy-paste the resources pointed to by all symlinks manually. For Android demo a number of raw resources
are symlinks. You can find these symlinks [here](/demo/android/app/src/main/res/raw). This includes all the keyword
files (i.e. files with `.ppn` extension) and also the model parameter file (i.e. file with `.pv` extension).

## Library Installation and Usage

You can add [PorcupineManager](/demo/android/porcupinemanager) as a dependency to
your Android app by consulting the documentation on
[Android Developers](https://developer.android.com/studio/projects/android-library.html#AddDependency) website.

[PorcupineManager](/demo/android/porcupinemanager) requires `RECORD_AUDIO` user permission. Be sure it is granted to your App.
Furthermore, Porcupine performs data-intensive operations such as recording/processing audio which should not be run on the
UI thread. Make sure you are following the [best practices for data-intensive operations](https://developer.android.com/training/multiple-threads/index.html).

The library can be initialized using

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

When initialized, input audio can be monitored using ```manager.start() ```. When done be sure to stop the manager using
```manager.stop()```. `modelFilePath` should point to [model parameter file](/lib/common/porcupine_params.pv) which contains
neural network parameters. This file is shipped separately to reduce the size of library while supporting all ABIs. Furthermore,
`keywordFilePath` should point to an Android keyword file available at [resources/keyword_files](/resources/keyword_files). Custom
keyword files for Android are provided with purchase of the commercial license. In order to purchase a commercial license please
 contact us at contact@picovoice.ai. For more information about keyword files refer to
[tools/optimizer](/tools/optimizer). Finally, `sensitivity` is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate. For more information about this parameter 
refer to [include/pv_porcupine.h](/include/pv_porcupine.h).

If you'd like to run the tests navigate to [androidTest](/demo/android/porcupinemanager/src/androidTest/java/ai/picovoice/porcupinemanager) using Android Studio,
right click, and select `Run Tests in ai.picovoice.porcupinemanager` from drop down menu.

