# Android Speech-To-Text Demo

Porcupine Wake Word engine with Android STT.

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

Open demo/android/STT with Android Studio. Launch the demo on your connected Android device or simulator.

1. Select the wake word using the dropdown
2. Press start
3. Say the wake word
4. Wait for the wake word to be detected
5. Say any phrase

E.g.:

> "Alexa"

The app will detect the wake word and start Android STT. The state should be in `Running STT` and the box will change colors to dark blue. Any phrases picked up will be added to the box.