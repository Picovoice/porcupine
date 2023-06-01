# porcupine-web-vue-demo

**NOTE**: Although this demo is using Vue 3, the [Vue Porcupine binding](https://github.com/Picovoice/porcupine/tree/master/binding/vue) works with both Vue 2 and Vue 3.

This demo application includes a sample `VoiceWidget` Vue component which uses the `porcupineMixin` Vue mixin which integrates components to allow listening for keywords. Porcupine keyword detections are handled via the `keywordCallback` function.

If you decline microphone permission in the browser, or another such issue prevents Porcupine from starting, the error will be displayed.

The widget shows the various loading and error events, as well as mounting/unmounting the `VoiceWidget` with a toggle, demonstrating the complete lifecycle of Porcupine with in a Vue app.

This project was bootstrapped with Vite. See the [Configuration Reference](https://vitejs.dev/config/).

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

The command-line output will provide you with a localhost link and port to open in your browser.

## Usage

1) Enter your AccessKey and press "Start Porcupine"
2) Select the keyword you wish to detect
3) Click start and say the keyword - detections will appear on the page
