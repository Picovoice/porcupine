# porcupine-angular-demo

This demo application includes a sample `VoiceWidget` Angular component which uses the `PorcupineService` Angular service to allow listening for keywords. Porcupine keyword detections are handled via the `keywordDetection$` event. Our VoiceWidget subscribes to this event and displays the results.

If you decline microphone permission in the browser, or another such issue prevents Porcupine from starting, the error will be displayed.

The widget shows the various loading and error events, as well as mounting/unmounting the `VoiceWidget` with a toggle, demonstrating the complete lifecycle of Porcupine with in an Angular app.

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) version 11.0.5.

## Install and Run

Use `yarn` or `npm` to install the dependencies, and the `start` script with a language code
to start a local web server hosting the demo in the language of your choice (e.g. `de` -> German, `ko` -> Korean).
To see a list of available languages, run `start` without a language code.

```console
yarn
yarn start ${LANGUAGE}
```

(or)

```console
npm install
npm run start ${LANGUAGE}
```

Open `http://localhost:4200/` to view it in the browser.

## Usage

1) Enter your AccessKey and press "Start Porcupine"
2) Select the keyword you wish to detect
3) Click start and say the keyword - detections will appear on the page
