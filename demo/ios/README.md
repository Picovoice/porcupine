# Porcupine iOS Demos

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## BackgroundService Demo

This demo runs microphone recording in the background and detects the wake word while the application is **not** in focus. The wake word will be detected as long as the application remains running in the background.

**The minimum iOS version required is 15.6.**

1) To run the demo, go to [demo/ios/BackgroundService](demo/ios/BackgroundService).

2) Open the `BackgroundService.xcodeproj` in XCode

3) Replace `let accessKey = "${YOUR_ACCESS_KEY_HERE}"` in the file [ViewModel.swift](./demo/ios/BackgroundService/BackgroundService/ViewModel.swift) with your `AccessKey`.

4) Run the demo with a simulator or connected iOS device

Open up the demo and press start. The demo listens for the wake word "Porcupine". You may leave the application but the application must be running in the background to be able to continuously be detecting for the wake word.

The demo uses Swift Package Manager to manage dependencies. The SPM will use `git clone --mirror` to clone the repository and result in a ~5GB download size. Please be patient with the downloading process.

## ForegroundApp Demo

This demo runs wake word detection while the application is in focus.

To run the foreground application demo:

1) Go to [ForegroundApp](./demo/ios/ForegroundApp) directory.

2) Open the `ForegroundApp.xcodeproj` in XCode

3) Replace `let accessKey = "${YOUR_ACCESS_KEY_HERE}"` in the file [ViewModel.swift](./demo/ios/ForegroundApp/ForegroundApp/ViewModel.swift) with your `AccessKey`.

4) Go to `Product > Scheme` and select the scheme for the language you would like to demo (e.g. `esDemo` -> Spanish Demo, `deDemo` -> German Demo)

5) Run the demo with a simulator or connected iOS device

The demo allows you to select any of the pre-built keywords for detection. Press start and say the selected keyword.

The demo uses Swift Package Manager to manage dependencies. The SPM will use `git clone --mirror` to clone the repository and result in a ~5GB download size. Please be patient with the downloading process.
