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

## AccessKey

The Porcupine SDK requires a valid `AccessKey` at initialization. `AccessKey`s act as your credentials when using Porcupine SDKs.
You can create your `AccessKey` for free. Make sure to keep your `AccessKey` secret.

To obtain your `AccessKey`:
1. Login or Signup for a free account on the [Picovoice Console](https://picovoice.ai/console/).
2. Once logged in, go to the [`AccessKey` tab](https://console.picovoice.ai/access_key) to create one or use an existing `AccessKey`.

## Usage

Import the Porcupine component and the Porcupine Web Worker component. Bind the worker to Porcupine like the demo `.vue` file below.

In this example we're passing in two keywords: "Grasshopper" and "Grapefruit" with sensitivities 0.65 and 0.4, respectively. The demo maintains an array of detections which is updated every time the Porcupine `ppn-keyword` event is fired.

```html
<template>
  <div class="voice-widget">
    <Porcupine
      v-bind:porcupineFactoryArgs="{
        accessKey: "AccessKey obtained from Picovoice Console(https://picovoice.ai/console/)",
        keywords: [
          { builtin: "Grasshopper", sensitivity: 0.5 },
          { builtin: "Grapefruit", sensitivity: 0.6 },
        ],
      }"
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

### Custom wake words

Each language includes a set of built-in keywords. The quickest way to get started is to use one of those. The builtin keywords are licensed under Apache-2.0 and are completely free to use.

Custom wake words are generated using [Picovoice Console](https://picovoice.ai/console/). They are trained from text using transfer learning into bespoke Porcupine keyword files with a `.ppn` extension. The target platform is WebAssembly (WASM), as that is what backs the Vue library.

The `.zip` file containes a `.ppn` file and a `_b64.txt` file which containes the binary model encoded with Base64. Copy the base64 and provide it as an argument to Porcupine as below. You will need to also provide a label so that Porcupine can tell you which keyword occurred ("Deep Sky Blue", in this case):

```html
<template>
  <div class="voice-widget">
    <Porcupine
      v-bind:porcupineFactoryArgs="[
        { base64: '/* Base64 representation of deep_sky_blue.ppn */', custom: 'Deep Sky Blue', sensitivity: 0.65 },
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

You may wish to store the base64 string in a separate JavaScript file and export it to keep your application code separate.
