# Porcupine Flutter Demo

To run the Porcupine demo on Android or iOS with Flutter, you must have the [Flutter SDK](https://flutter.dev/docs/get-started/install) installed on your system. Once installed, you can run `flutter doctor` to determine any other missing requirements for your relevant platform. Once your environment has been set up, launch a simulator or connect an Android/iOS device. 

## AccessKey

All demos require a valid Picovoice `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://picovoice.ai/console/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

Once you obtain your `AccessKey`, replace it in [lib/main.dart](lib/main.dart) file:

```dart
final String accessKey = "{YOUR_ACCESS_KEY_HERE}"; // AccessKey obtained from Picovoice Console (https://picovoice.ai/console/)
```

## Usage

Run the following command from [demo/flutter](/demo/flutter) to build and deploy the demo to your device:
```console
flutter run
```


