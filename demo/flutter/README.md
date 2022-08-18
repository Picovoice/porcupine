# Porcupine Flutter Demo

To run the Porcupine demo on Android or iOS with Flutter, you must have the [Flutter SDK](https://flutter.dev/docs/get-started/install) installed on your system. Once installed, you can run `flutter doctor` to determine any other missing requirements for your relevant platform. Once your environment has been set up, launch a simulator or connect an Android/iOS device. 

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

Replace your `AccessKey` in [lib/main.dart](lib/main.dart) file:

```dart
final String accessKey = "{YOUR_ACCESS_KEY_HERE}"; // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
```

Run the following command from [demo/flutter](.) to build and deploy the demo to your device:
```console
flutter run
```
