# Porcupine

[![Build Status](https://travis-ci.org/Picovoice/Porcupine.svg?branch=master)](https://travis-ci.org/Picovoice/Porcupine)

Porcupine is a self-service, highly-accurate, and lightweight **wake word** detection engine. It enables developers to
build always-listening voice-enabled applications/platforms. Porcupine is

* self-service. Developers are empowered to **choose any wake word** and build its model **within seconds**.
* using **deep neural networks** trained in **real-world situations** (i.e. noise and reverberation).
* compact and computationally-efficient making it suitable for **IoT** applications.
* **cross platform**. It is implemented in pure ANSI C. Currently **Raspberry Pi**, **Android**, **iOS**, **Linux**, and 
**Mac** are supported.
* **open-source**. Anything you find in this repository is Apache 2.0 licensed.

## Try It Out

Try out Porcupine by downloading it's [Android demo application](https://play.google.com/store/apps/details?id=ai.picovoice.porcupine.demo&hl=en).
The demo application allows you to test Porcupine on a variety of wake words in any environment.

## Structure of Repository

Porcupine is shipped as an ANSI C precompiled library. The binary files for supported platforms are located under
[lib/](/lib) and header files are at [include/](/include). Currently, Raspberry Pi, Android, iOS, Linux, and Mac are supported.
If you wish to run Porcupine on any other platform, please contact us at help@picovoice.ai.

Bindings are available at [binding/](/binding) to facilitate usage from higher-level languages/platforms. Demo
applications are at [demo/](/demo). When possible, use one of the demo applications as a starting point for your own implementation.

[tools/](/tools) contains utility programs. Finally, [resources/](/resources) is a placeholder for data used by
various applications within the repository. Some of the files under resources as stored using [Git LFS](https://git-lfs.github.com/).
Please be sure to [install Git LFS](https://help.github.com/articles/installing-git-large-file-storage/) before cloning the repository. 

Below is a quick walk-through of the repository. For detailed instructions please visit relevant pages. Throughout the
documentation it is assumed that the current working directory is the root of repository.

## Running Demo Applications

### Python Demo Application (Raspberry Pi, Linux, Mac)

This [demo application](/demo/python) allows testing Porcupine using computer's microphone. It opens an input audio
stream, monitors it using Porcupine's library, and logs the detection events into the console. Below is an
example of running the demo for wake word **Alexa** from the command line. Replace ```${SYSTEM}``` with the name of
the operating system on your machine (i.e. linux or mac).

```bash
python demo/python/porcupine_demo.py --keyword_file_path resources/keyword_files/alexa_${SYSTEM}.ppn
```

### Android Demo Application

Using [Android Studio](https://developer.android.com/studio/index.html) open [demo/android](/demo/android) as an android
project and then run the application. Note that you need an android phone with developer options enabled connected to
your machine in order to run the application.

## Creating Keyword Files

Porcupine enables developers to build models for any wake word. This is done using Porcupine's optimizer utility.
It finds optimal model hyper-parameters for a given wake word and stores these parameters in a, so-called, keyword file.
You could create your own keyword file using the [Porcupine's optimizer](/tools/optimizer) from the command line

```bash
tools/optimizer/${SYSTEM}/${MACHINE}/pv_porcupine_optimizer -r resources/ -w ${WAKE_WORD} \
-p ${TARGET_SYSTEM} -o ${OUTPUT_DIRECTORY}
```

In the above example replace ```${SYSTEM}``` and ```${TARGET_SYSTEM}``` with current and target (runtime) operating
systems (linux or mac). ```${MACHINE}``` is the CPU architecture of current machine (x86_64 or i386). ```${WAKE_WORD}```
is the chosen wake word. Finally, ```${OUTPUT_DIRECTORY}``` is the output directory where keyword file will be stored.

## Integration

Below are code snippets showcasing how Porcupine can be integrated into different applications.

### C

Porcupine is implemented in ANSI C and therefore can be directly linked to C applications.
[include/pv_porcupine.h](/include/pv_porcupine.h) and [include/picovoice.h](include/picovoice.h) header files contain 
relevant information. An instance of Porcupine object can be constructed as follows

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
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

Now the ```handle``` can be used to monitor incoming audio stream. Porcupine accepts single channel, 16-bit PCM audio.
The sample rate can be retrieve using ```pv_sample_rate()```. Finally, Porcupine accepts input audio in consecutive chunks
(aka frames) the length of each frame can be retrieved using ```pv_porcupine_frame_length()```

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

Finally, when done be sure to release resources acquired.

```c
pv_porcupine_delete(handle);
```

### Python

[/binding/python/porcupine.py](/binding/python/porcupine.py) provides Python binding for Porcupine library. Below is a
quick demonstration of how to construct an instance of it.

```python
library_path = ... # Path to Porcupine's C library available under lib/${SYSTEM}/${MACHINE}/
model_file_path = ... # It is available at lib/common/porcupine_params.pv
keyword_file_path = ...
sensitivity = 0.5
handle = Porcupine(library_path, model_file_path, keyword_file_path, sensitivity)
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

When initialized, valid sample rate can be obtained using ```handle.sample_rate```. Expected frame length 
(number of audio samples in an input array) is ```handle.frame_length```. The object can be used to monitor
incoming audio as below

```python
def get_next_audio_frame():
    pass
    
while True:
    pcm = get_next_audio_frame()
    result = handle.process(pcm)
    if result:
        # detection event logic/callback
        pass
```

Finally, when done be sure to explicitly release the resources as the binding class does not rely on the garbage
collector

```python
handle.delete()
```

### Android

There are two possibilities for integrating Porcupine into your Android's application.

#### Binding

[Porcupine](/binding/android/porcupine/src/main/java/ai/picovoice/porcupine/Porcupine.java) provides a binding for 
Android using [JNI](https://docs.oracle.com/javase/7/docs/technotes/guides/jni/). It can be initialized using

```java
    final String modelFilePath = ... // It is available at lib/common/porcupine_params.pv
    final String keywordFilePath = ...
    final float sensitivity = 0.5f;
    
    Porcupine porcupine = new Porcupine(modelFilePath, keywordFilePath, sensitivity);
```

Sensitivity is the parameter that enables developers to trade miss rate for false alarm. It is a floating number within
[0, 1]. A higher sensitivity reduces miss rate at cost of increased false alarm rate.

Once initialized, ```porcupine``` can be used to monitor incoming audio

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

[Android demo application](/demo/android) provides a high-level API for integrating Porcupine into Android applications.
The [PorcupineManager](/demo/android/porcupinemanager/src/main/java/ai/picovoice/porcupinemanager/PorcupineManager.java) class 
manages all activities related to creating an input audio stream, feeding it into Porcupine's library, and
invoking a user-provided detection callback. The class can be initialized as below

```java
    final String modelFilePath = ... # It is available at lib/common/porcupine_params.pv
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
 
When initialized, input audio can be monitored using `manager.start() `. When done be sure to stop the manager using
`manager.stop()`.

## License

Anything you find in this repository is licensed under Apache 2.0. For obtaining a commercial license please contact us
at sales@picovoice.ai.

