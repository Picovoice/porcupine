# Android Activity Demo

This Android demo runs Porcupine in the foreground (i.e., when the app is in focus).

## AccessKey

All Android demos require a valid Picovoice `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://picovoice.ai/console/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

Copy your AccessKey into the `ACCESS_KEY` variable in `MainActivity.java` before building the demo.

## Usage

Launch the demo on your phone using Android Studio.

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