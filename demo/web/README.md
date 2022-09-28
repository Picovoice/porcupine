# porcupine-web-demo

This is a basic demo to show how to use Porcupine for web browsers, using the IIFE version of the library (i.e. an HTML
script tag). It instantiates a Porcupine worker engine and uses it with
the [@picovoice/web-voice-processor](https://www.npmjs.com/package/@picovoice/web-voice-processor) to access (and
automatically downsample) microphone audio.

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using
Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Install & run

Use `yarn` or `npm` to install the dependencies, and the `start` script to start a local web server hosting the demo.

```console
yarn
yarn start
```

(or)

```console
npm install
npm run start
```

Open `localhost:5000` in your web browser, as hinted at in the output:

```console
Available on:
  http://localhost:5000
Hit CTRL-C to stop the server
```

Wait until Porcupine and the WebVoiceProcessor have initialized. Say any phrase and Porcupine will display detection
events on the screen.
