# Porcupine Unity Demo 

The Porcupine demo for Unity is a multi-platform demo that runs on:

- Desktop Standalone (Windows, macOS and Linux x86_64)
- iOS 13.0+
- Android 5.0+

Additionally, you will need a version of Unity that is 2017.4 or higher.

## Usage
The easiest way to run the demo is to simply import the [Porcupine Unity package](../../binding/unity/porcupine-3.0.0.unitypackage) into your project, open the PorcupineDemo scene and hit play. To run on other platforms or in the player, go to _File > Build Settings_, choose your platform and hit the `Build and Run` button.

Once the demo launches, press the `Start Listening` button and try out any of the listed wake words.

## AccessKey

All demos require a valid Picovoice `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://console.picovoice.ai/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

Replace the `AccessKey` in [`PorcupineDemo.cs`](PorcupineDemo.cs) with the `AccessKey` obtained above:

```csharp
private const string ACCESS_KEY = "${YOUR_ACCESS_KEY_HERE}"; // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)
```
