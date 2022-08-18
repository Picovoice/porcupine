# Android Demos

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Activity

This demo is intended for applications that need to do voice recognition when in focus.

Copy your AccessKey into the `ACCESS_KEY` variable in `MainActivity.java` before building the demo.

## Service

This demo is intended for applications that need to do voice recognition in the background.

**NOTE:** If running the Porcupine as a service on Android < 10, you will need to stop the service before attempting to record audio in another application. This is a limitation of the Android OS that is documented [here](https://developer.android.com/guide/topics/media/sharing-audio-input).

Copy your AccessKey into the `ACCESS_KEY` variable in `PorcupineService.java` before building the demo.

## Speech to Text

This demo is intended for applications that use Porcupine Wake Work engine with Android's STT recognition service.

Copy your AccessKey into the `ACCESS_KEY` variable in `MainActivity.java` before building the demo.

## Use Custom Wake Words (.ppn files)

The demos use built-in wake words that are included with Porcupine (e.g. "Computer", "Alexa", "Picovoice"). To add a custom wake word or model file to your application, add the files to your assets folder (`src/main/assets`) and then pass the path to the Porcupine Builder:

```java
// in this example our files are located at '/assets/picovoice_files/keyword.ppn' and '/assets/picovoice_files/model.pv' 
try {    
    Porcupine porcupine = new Porcupine.Builder()
                        .setKeywordPath("picovoice_files/keyword.ppn")
                        .setModelPath("picovoice_files/model.pv")
                        .build(context);
} catch (PorcupineException e) { }
```


## Non-English Wake Words

In order to detect non-English wake words you need to use the corresponding model file (file with `.pv` extension). The model files for all supported languages are available in [lib/common](../../lib/common).
