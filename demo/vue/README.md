# porcupine-web-vue-demo

**NOTE**: Although this demo is using Vue 3, the [Vue Porcupine binding](https://github.com/Picovoice/porcupine/tree/master/binding/vue) works with both Vue 2 and Vue 3.

This demo application includes a sample `VoiceWidget` Vue component which uses the `porcupineMixin` Vue mixin which integrates components to allow listening for keywords. Porcupine keyword detections are handled via the `keywordCallback` function.

If you decline microphone permission in the browser, or another such issue prevents Porcupine from starting, the error will be displayed.

The widget shows the various loading and error events, as well as mounting/unmounting the `VoiceWidget` with a toggle, demonstrating the complete lifecycle of Porcupine with in a Vue app.

This project was bootstrapped with Vue CLI. See the [Configuration Reference](https://cli.vuejs.org/config/).

## Install & run

```console
yarn
yarn start
```

(or)

```console
npm install
npm run start
```

The command-line output will provide you with a localhost link and port to open in your browser.

## Try Porcupine

Say the wake phrases:

> "Grapefruit"
or

> "Grasshopper"

The detections will appear on screen.
