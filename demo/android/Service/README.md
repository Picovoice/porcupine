# Android Service Demo

This Android demo runs Porcupine as a background service. You can switch apps or put your phone to sleep and it will continue listening for the wake word ("Computer"). When the wake word is detected, you'll receive a notification message.

## AccessKey

All Android demos require a valid Picovoice `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://picovoice.ai/console/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

Copy your AccessKey into the `ACCESS_KEY` variable in `PorcupineService.java` before building the demo.

## Usage

Launch the demo on your phone and press the "Start" button.

Say:

> "Computer"

When it detects the wake word, it will send a system notification.
