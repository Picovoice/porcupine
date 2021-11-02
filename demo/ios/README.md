# Porcupine iOS Demos

## AccessKey

The Porcupine SDK requires a valid `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://picovoice.ai/console/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

## BackgroundService Demo

This demo runs microphone recording in the background and detects the wake word while the application is **not** in focus. The wake word will be detected as long as the application remains running in the background.

**The minimum iOS version required is 10.0.**

To run the background service demo, go to [BackgroundService](./BackgroundService) directory. Then run:

```console
pod install
```

Replace `let accessKey = ""` in the file [ViewController.swift](./BackgroundService/PorcupineBackgroundServiceDemo/ViewController.swift) with your `AccessKey`.

Open `PorcupineBackgroundServiceDemo.xcworkspace` and run the demo.

Open up the demo and press start. The demo listens for the wake word "Porcupine". You may leave the application but the application must be running in the background to be able to continuously be detecting for the wake word.

## ForegroundApp Demo

This demo runs wake word detection while the application is in focus. 

To run the foreground application demo, go to [ForegroundApp](./ForegroundApp) directory. Then run:

```console
pod install
```

Replace `let accessKey = ""` in the file [ViewController.swift](./ForegroundApp/PorcupineForegroundAppDemo/ViewController.swift) with your `AccessKey`.

Open the `PorcupineForegroundAppDemo.xcworkspace` and run the demo.

The demo allows you to select any of our built-in keywords for detection. Press start and say the selected keyword.

## Running the On-Device Unit Tests

Open `PorcupineForegroundAppDemo.xcworkspace` with XCode and run the tests with `Product > Test`.

