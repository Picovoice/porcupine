# Android Activity Demo

This Android demo runs Porcupine in the foreground (i.e., when the app is in focus).

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Setup

1. Open the project in Android Studio
2. Copy your `AccessKey` into the `ACCESS_KEY` variable in [MainActivity.java](./porcupine-activity-demo-app/src/main/java/ai/picovoice/porcupine/demo/MainActivity.java).
3. Go to `Build > Select Build Variant...` and select the language you would like to run the demo in (e.g. enDebug -> English, itRelease -> Italian)
4. Build and run on an installed simulator or a connected Android device

## Usage

1. Select the wake word using the dropdown
2. Press start
3. Say the wake word

E.g.:

> "Alexa"

The app background color will change to indicate a detection occurred.

## Running the Instrumented Unit Tests

Ensure you have an Android device connected or simulator running. Then run the following from the terminal:

```console
cd demo/android/Activity
./copy_test_resources.sh
./gradlew connectedAndroidTest
```

The test results are stored in `porcupine-activity-demo-app/build/reports`.
