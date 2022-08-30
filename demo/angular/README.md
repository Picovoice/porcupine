# porcupine-angular-demo

This demo application includes a sample `VoiceWidget` Angular component which uses the `PorcupineService` Angular service to allow listening for keywords. Porcupine keyword detections are handled via the `keywordDetection$` event. Our VoiceWidget subscribes to this event and displays the results.

If you decline microphone permission in the browser, or another such issue prevents Porcupine from starting, the error will be displayed.

The widget shows the various loading and error events, as well as mounting/unmounting the `VoiceWidget` with a toggle, demonstrating the complete lifecycle of Porcupine with in an Angular app.

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) version 11.0.5.

## Install and run

Use `yarn` or `npm`:

```console
yarn
yarn start
```

(or)

```console
npm install
npm run start
```

Open `http://localhost:4200/` to view it in the browser.

## Try Porcupine

The demo is listening for the wake word "Alexa or Porcupine". Say the wake word:

> "Porcupine"

Detections will appear on the page.
