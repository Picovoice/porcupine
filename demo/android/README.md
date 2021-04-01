## Activity

This demo is intended for applications that need to do voice recognition when in focus.

## Service

This demo is intended for application that need to do voice recognition and for an extended period of time.

**NOTE:** If running the Porcupine as a service on Android < 10, you will need to stop the service before attempting to record audio in another application. This is a limitation of the Android OS that is documented [here](https://developer.android.com/guide/topics/media/sharing-audio-input).

## How to Use Custom Wake Words (.ppn files)

To add a custom wake word to your Android application a couple of extra steps must be taken. First, add your .ppn file to the `/res/raw` folder. All resources are compressed when the build system creates an APK, so you will have to extract your ppn file first before using it:

```java
// in this example our file located at '/res/raw/keyword.ppn'
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

In order to detect non-English wake words you need to use the corresponding model file. The model files for all
supported languages are available [here](/lib/common).
