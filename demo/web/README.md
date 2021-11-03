# porcupine-web-demo

This is a basic demo to show how to use Porcupine for web browsers, using the IIFE version of the library (i.e. an HTML script tag). It instantiates a Porcupine worker engine and uses it with the [@picovoice/web-voice-processor](https://www.npmjs.com/package/@picovoice/web-voice-processor) to access (and automatically downsample) microphone audio.

## AccessKey

The Porcupine libraries require a valid `AccessKey` at initialization. `AccessKey`s act as your credentials.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://picovoice.ai/console/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

## Install & run

Use `yarn` or `npm` to install the dependencies, and the `start` script to start a local web server hosting the demo.

```console
yarn
yarn start
```

(or)

```console
npm
npm run start
```

Open `localhost:5000` in your web browser, as hinted at in the output:

```console
   ┌──────────────────────────────────────────────────┐
   │                                                  │
   │   Serving!                                       │
   │                                                  │
   │   - Local:            http://localhost:5000      │
   │   - On Your Network:  http://192.168.1.69:5000   │
   │                                                  │
   │   Copied local address to clipboard!             │
   │                                                  │
   └──────────────────────────────────────────────────┘
```

Wait until Porcupine and the WebVoiceProcessor have initialized. Say "Picovoice" and Porcupine will display detection events on the screen.
