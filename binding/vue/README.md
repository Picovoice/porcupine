# porcupine-web-vue

Renderless Vue component for Porcupine for Web.

## Porcupine

Porcupine is a highly accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications.

> Porcupine wake word models for all major voice assistants (`Alexa`, `Hey Google`, `Ok Google`, and `Hey Siri`) are
> available for free (under Apache 2.0)

## Compatibility

This library is compatible with Vue:
- Vue.js 2.6.11+.
- Vue.js 3.0.0+.

The Picovoice SDKs for Web are powered by WebAssembly (WASM), the Web Audio API, and Web Workers.

All modern browsers (Chrome/Edge/Opera, Firefox, Safari) are supported, including on mobile. Internet Explorer is _not_ supported.

Using the Web Audio API requires a secure context (HTTPS connection), except for `localhost`, for local development.

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

Import the Porcupine mixin and the Porcupine Web Worker component. When a component adds the `porcupineMixin`, the variable `$porcupine` is computed with the following functions:

- `init`: initializes Porcupine.
- `start`: starts processing audio and detecting keywords.
- `pause`: stops processing audio.
- `delete`: cleans up used resources.

In this example we're passing in two keywords: "Grasshopper" and "Grapefruit" with sensitivities 0.65 and 0.4, respectively. The demo maintains an array of detections which is updated every time the Porcupine detects a keyword.

```html
<script lang="ts">
import porcupineMixin from "@picovoice/porcupine-web-vue";
import { PorcupineWorkerFactoryEn } from "@picovoice/porcupine-web-en-worker";

export default {
  name: "VoiceWidget",
  mixins: [porcupineMixin],
  data: function() {
    return {
      detections: [] as string[],
      isError: false,
      isLoaded: false,
      factory: PorcupineWorkerFactoryEn,
      factoryArgs: {
        accessKey: 'AccessKey obtained from Picovoice Console(https://picovoice.ai/console/)',
        keywords: [
          { builtin: 'Grasshopper', sensitivity: 0.5 },
          { builtin: 'Grapefruit', sensitivity: 0.6 },
        ],
      }
    };
  },
  async created() {
    await this.$porcupine.init(
      this.factoryArgs,     // Porcupine factory arguments
      this.factory,         // Porcupine Web Worker component
      this.ppnKeywordFn,    // Callback invoked after detection of keyword
      this.ppnReadyFn,      // Callback invoked after loading Porcupine
      this.ppnErrorFn       // Callback invoked in an error occurs while initializing Porcupine
    );
  },
  methods: {
    start: function () {
      if (this.$porcupine.start()) {
        this.isListening = !this.isListening;
      }
    },
    pause: function () {
      if (this.$porcupine.pause()) {
        this.isListening = !this.isListening;
      }
    },
    ppnReadyFn: function() {
      this.isLoaded = true;
    },
    ppnKeywordFn: function(data: string) {
      this.detections = [...this.detections, data.keywordLabel];
    },
    ppnErrorFn: function(error: Error) {
      this.isError = true;
      this.errorMessage = error.toString();
    },
  },
};
</script>
```

### Custom wake words

Each language includes a set of built-in keywords. The quickest way to get started is to use one of those. The builtin keywords are licensed under Apache-2.0 and are completely free to use.

Custom wake words are generated using [Picovoice Console](https://picovoice.ai/console/). They are trained from text using transfer learning into bespoke Porcupine keyword files with a `.ppn` extension. The target platform is WebAssembly (WASM), as that is what backs the Vue library.

The `.zip` file contains a `.ppn` file and a `_b64.txt` file which contains the binary model encoded with Base64. Copy the base64 and provide it as an argument to Porcupine as below. You will need to also provide a label so that Porcupine can tell you which keyword occurred ("Deep Sky Blue", in this case):

```typescript
factoryArgs: {
  accessKey: 'AccessKey obtained from Picovoice Console(https://picovoice.ai/console/)',
  keywords: [
    { custom: base64: '/* Base64 representation of deep_sky_blue.ppn */', custom: 'Deep Sky Blue', sensitivity: 0.65 },
  ],
}
```

You may wish to store the base64 string in a separate JavaScript file and export it to keep your application code separate.
