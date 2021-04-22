# porcupine-web-vue-demo

This demo application includes a sample `VoiceWidget` Vue component which uses the `Porcupine` renderless Vue component service to allow listening for keywords. Porcupine keyword detections are handled via the `ppn-keyword` event. Our VoiceWidget subscribes to this event and displays the results.

The demo uses dynamic imports to split the VoiceWidget away from the main application bundle. This means that the initial download size of the Vue app will not be impacted by the ~1-2MB requirement of Porcupine. While small for all-in-one offline Voice AI, the size is large for an intial web app load.

If you decline microphone permission in the browser, or another such issue prevents Porcupine from starting, the error will be displayed.

The widget shows the various loading and error events, as well as mounting/unmounting the `VoiceWidget` with a toggle, demonstrating the complete lifecycle of Porcupine with in a Vue app.

This project was bootstrapped with Vue CLI. See the [Configuration Reference](https://cli.vuejs.org/config/).

## Install & run

```console
yarn
yarn serve
```

(or)

```console
npm install
npm run serve
```

The command-line output will provide you with a localhost link and port to open in your browser.

## Try Porcupine

Say the wake phrases:

> "Grapefruit" 
or

> "Grasshopper"

The detections will appear on screen.
