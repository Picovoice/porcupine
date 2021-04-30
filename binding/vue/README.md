# porcupine-web-vue

Renderless Vue component for Porcupine for Web.

## Porcupine

Porcupine is a highly accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications.

> Porcupine wake word models for all major voice assistants (`Alexa`, `Hey Google`, `Ok Google`, and `Hey Siri`) are
> available for free (under Apache 2.0)

## Compatibility

This library is compatible with Vue 3.

The Picovoice SDKs for Web are powered by WebAssembly (WASM), the Web Audio API, and Web Workers.

All modern browsers (Chrome/Edge/Opera, Firefox, Safari) are supported, including on mobile. Internet Explorer is _not_ supported.

Using the Web Audio API requires a secure context (HTTPS connection), with the exception of `localhost`, for local development.

## Installation

Install the package using `npm` or `yarn`. You will also need to add one of the `@picovoice/porcupine-web-**-worker` series of packages for the specific language model:

E.g. English:

```console
yarn add @picovoice/porcupine-web-vue @picovoice/porcupine-web-en-worker @picovoice/web-voice-processor
```

## Usage

Import the Porcupine component and the Porcupine Web Worker component. Bind the worker to Porcupine like the demo `.vue` file below.

In this example we're passing in two keywords: "Grasshopper" and "Grapefruit" with sensitivities 0.65 and 0.4, respectively. The demo maintains an array of detections which is updated every time the Porcupine `ppn-keyword` event is fired.

```html
<template>
  <div class="voice-widget">
    <Porcupine
      v-bind:porcupineFactoryArgs="[
        { builtin: 'Grasshopper', sensitivity: 0.65 },
        { builtin: 'Grapefruit', sensitivity: 0.4 },
      ]"
      v-bind:porcupineFactory="factory"
      v-on:ppn-ready="ppnReadyFn"
      v-on:ppn-keyword="ppnKeywordFn"
      v-on:ppn-error="ppnErrorFn"
    />
    <h3>Keyword Detections:</h3>
    <ul v-if="detections.length > 0">
      <li v-for="(item, index) in detections" :key="index">{{ item }}</li>
    </ul>
  </div>
</template>
```

```javascript
<script>
import Porcupine from "@picovoice/porcupine-web-vue";
import { PorcupineWorkerFactoryEn } from "@picovoice/porcupine-web-en-worker";

export default {
  name: "VoiceWidget",
  components: {
    Porcupine,
  },
  data: function() {
    return {
      detections: [],
      isError: null,
      isLoaded: false,
      factory: PorcupineWorkerFactoryEn,
    };
  },
  methods: {
    ppnReadyFn: function() {
      this.isLoaded = true;
    },
    ppnKeywordFn: function(data) {
      this.detections = [...this.detections, data.keywordLabel];
    },
    ppnErrorFn: function(data) {
      this.isError = true;
      this.errorMessage = data.toString();
    },
  },
};
</script>
```

## Events

The Porcupine component will emit the following events:

| Event         | Data                                                                  | Description                                                                                         |
| ------------- | --------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| "ppn-loading" |                                                                       | Porcupine has begun loading                                                                         |
| "ppn-ready"   |                                                                       | Porcupine has finished loading & the user has granted microphone permission: ready to process voice |
| "ppn-keyword" | The label of the keyword (e.g. "Grasshopper")                         | Porcupine has detected a keyword                                                                    |
| "ppn-error"   | The error that was caught (e.g. "NotAllowedError: Permission denied") | An error occurred while Porcupine or the WebVoiceProcessor was loading                              |
