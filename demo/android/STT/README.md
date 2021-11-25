# Android Speech-To-Text Demo

Porcupine Wake Word engine with Android STT.

## AccessKey

All Android demos require a valid Picovoice `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://picovoice.ai/console/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

Copy your AccessKey into the `ACCESS_KEY` variable in `MainActivity.java` before building the demo.

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