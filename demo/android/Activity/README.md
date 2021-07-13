# Android Activity Demo

This Android demo runs Porcupine in the foreground (i.e., when the app is in focus).

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