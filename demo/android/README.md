# Android Demos

## Activity

This demo is intended for applications that need to do voice recognition when in focus.

## Service

This demo is intended for applications that need to do voice recognition in the background.

**NOTE:** If running the Porcupine as a service on Android < 10, you will need to stop the service before attempting to record audio in another application. This is a limitation of the Android OS that is documented [here](https://developer.android.com/guide/topics/media/sharing-audio-input).

## Speech to Text

This demo is intended for applications that use Porcupine Wake Work engine with Android's STT recognition service.

## Use Custom Wake Words (.ppn files)

The demos use built-in wake words that are included with Porcupine (e.g. "Computer", "Alexa", "Picovoice"). To add a custom wake word (e.g. one created by [Picovoice Console](https://picovoice.ai/console/) to your Android application a couple of extra steps must be taken:

1. Add your `.ppn` file to the `/res/raw` folder.
1. All resources are compressed when the build system creates an APK, so you will have to extract your ppn file first before using it:

```java
// in this example, our file located at '/res/raw/keyword.ppn'
try (
        InputStream is = new BufferedInputStream(
            getResources().openRawResource(R.raw.keyword), 256);
        OutputStream os = new BufferedOutputStream(
            openFileOutput("keyword.ppn", Context.MODE_PRIVATE), 256)
) {
    int r;
    while ((r = is.read()) != -1) {
        os.write(r);
    }
    os.flush();
}
```

## Non-English Wake Words

In order to detect non-English wake words you need to use the corresponding model file (file with `.pv` extension). The model files for all supported languages are available in [lib/common](/lib/common).
