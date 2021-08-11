# Porcupine iOS Demo

## BackgroundService Demo

**The minimum iOS version required is 10.0.**

To run the background service demo, go to [BackgroundService](./BackgroundService) directory. Then run:

```console
pod install
```

Open `PorcupineBackgroundServiceDemo.xcworkspace` and run the demo.

Open up the demo and press start. The demo listens for the wake word "Porcupine". You may leave the application but the application must be running in the background to be able to continuously be detecting for the wake word.

## ForegroundApp Demo

To run the background service demo, go to [ForegroundApp](./ForegroundApp) directory. Then run:

```console
pod install
```

Open the `PorcupineForegroundAppDemo.xcworkspace` and run the demo.

The demo allows you to select any of our built-in keywords for detection. Press start and say the selected keyword.

## Running the On-Device Unit Tests

Open `PorcupineForegroundAppDemo.xcworkspace` with XCode and run the tests with `Product > Test`.

