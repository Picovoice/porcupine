# Android Service Demo

This Android demo runs Porcupine as a background service. You can switch apps or put your phone to sleep and it will continue listening for the wake word ("Computer"). When the wake word is detected, you'll receive a notification message.

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Usage

Launch the demo on your phone and press the "Start" button.

Say:

> "Computer"

When it detects the wake word, it will send a system notification.
