# porcupine-web

The Picovoice Porcupine library for web browsers, powered by WebAssembly. Intended (but not required) to be used with the [@picovoice/web-voice-processor](https://www.npmjs.com/package/@picovoice/web-voice-processor), [Angular](https://www.npmjs.com/package/@picovoice/porcupine-web-angular), [React](https://www.npmjs.com/package/@picovoice/porcupine-web-react), and [Vue](https://www.npmjs.com/package/@picovoice/porcupine-web-vue) packages.

This library processes always-listening voice commands in-browser, offline. All processing is done via WebAssembly and Workers in a separate thread.

Looking for Porcupine on NodeJS? See the [@picovoice/porcupine-node](https://www.npmjs.com/package/@picovoice/porcupine-node) package.

## Compatibility

- Chrome / Edge
- Firefox
- Safari

This library requires several modern browser features: WebAssembly, Web Workers, and promises. Internet Explorer will _not_ work.

If you are using this library with the [@picovoice/web-voice-processor](https://www.npmjs.com/package/@picovoice/web-voice-processor) to access the microphone, that requires some additional browser features like Web Audio API. Its overall browser support is approximately the same.

## Packages

The Porcupine SDK for Web is split into multiple packages due to each language including the entire Voice AI model, which is of nontrivial size. There are separate worker and factory packages as well, due to the complexities with bundling an "all-in-one" web workers without bloating bundle sizes. Import each as required.

Any Porcupine keyword files (`.ppn` files) generated from [Picovoice Console](https://picovoice.ai/console/) must be trained for the WebAssembly (WASM) platform and match the language of the instance you create. The `.zip` file contains a `.ppn` file and a `_b64.txt` file which contains the binary model encoded with Base64. The Base64 encoded models can then be passed into the Porcupine `create` function as arguments.

### Workers 

For typical cases, use the worker packages. Worker packages create complete `PorcupineWorker` instances that can be immediately used with [@picovoice/web-voice-processor](https://www.npmjs.com/package/@picovoice/web-voice-processor) and with the [Angular](https://www.npmjs.com/package/@picovoice/porcupine-web-angular), [React](https://www.npmjs.com/package/@picovoice/porcupine-web-react), and [Vue](https://www.npmjs.com/package/@picovoice/porcupine-web-vue) packages.

* [@picovoice/porcupine-web-de-worker](https://www.npmjs.com/package/@picovoice/porcupine-web-de-worker)
* [@picovoice/porcupine-web-en-worker](https://www.npmjs.com/package/@picovoice/porcupine-web-en-worker)
* [@picovoice/porcupine-web-es-worker](https://www.npmjs.com/package/@picovoice/porcupine-web-es-worker)
* [@picovoice/porcupine-web-fr-worker](https://www.npmjs.com/package/@picovoice/porcupine-web-fr-worker)

### Factories

Factory packages allow you to create instances of `Porcupine` directly on the main thread.

* [@picovoice/porcupine-web-de-factory](https://www.npmjs.com/package/@picovoice/porcupine-web-de-factory)
* [@picovoice/porcupine-web-en-factory](https://www.npmjs.com/package/@picovoice/porcupine-web-en-factory)
* [@picovoice/porcupine-web-es-factory](https://www.npmjs.com/package/@picovoice/porcupine-web-es-factory)
* [@picovoice/porcupine-web-fr-factory](https://www.npmjs.com/package/@picovoice/porcupine-web-fr-factory)

## Installation & Usage

### AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret. 
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

### Worker

To obtain a `PorcupineWorker`, we can use the static `create` factory method from the PorcupineWorkerFactory. Here is a complete example that:

1. Obtains a `PorcupineWorker` from the `PorcupineWorkerFactory` (in this case, English) to listen for the built-in English wake word "Picovoice"
1. Responds to the wake word detection event by setting the worker's `onmessage` event handler
1. Creates a `WebVoiceProcessor` to obtain microphone permission and forward microphone audio to the `PorcupineWorker`

E.g.:

```console
yarn add @picovoice/web-voice-processor @picovoice/porcupine-web-en-worker
```

```javascript
import { WebVoiceProcessor } from "@picovoice/web-voice-processor"
import { PorcupineWorkerFactory } from "@picovoice/porcupine-web-en-worker";

// The worker will call the callback function upon a detection event
// Here we tell it to log it to the console
function keywordDetectionCallback(keyword) {
  console.log(`Porcupine detected ${keyword}`);
}

// If during processing an audio stream, anything goes wrong, the worker will
// call the error callback and pass the error to it.
function processErrorCallback(error) {
  console.error(error); 
}

async function startPorcupine()
  // Create a Porcupine Worker (English language) to listen for 
  // the built-in keyword "Picovoice", at a sensitivity of 0.65
  // Note: you receive a Worker object, _not_ an individual Porcupine instance
  const accessKey = // .. AccessKey string obtained from Picovoice Console (https://picovoice.ai/console/)
  const porcupineWorker = await PorcupineWorkerFactory.create(
    accessKey,
    [{builtin: "Picovoice", sensitivity: 0.65}],
    keywordDetectionCallback,
    processErrorCallback
  );

  // Start up the web voice processor. It will request microphone permission 
  // and immediately (start: true) start listening.
  // It downsamples the audio to voice recognition standard format (16-bit 16kHz linear PCM, single-channel)
  // The incoming microphone audio frames will then be forwarded to the Porcupine Worker
  // n.b. This promise will reject if the user refuses permission! Make sure you handle that possibility.
  const webVp = await WebVoiceProcessor.init({
    engines: [porcupineWorker],
    start: true,
  });
  }


}
startPorcupine()

...

// Finished with Porcupine? Release the WebVoiceProcessor and the worker.
if (done) {
  webVp.release()
  porcupineWorker.sendMessage({command: "release"}) 
}

```

**Important Note**: Because the workers are all-in-one packages that run an entire machine learning inference model in WebAssembly, they are approximately 1-2 MB in size. While this is tiny for a speech recognition model, it's large for web delivery. Because of this, you likely will want to use dynamic `import()` instead of static `import {}` to reduce your app's starting bundle size. See e.g. https://webpack.js.org/guides/code-splitting/ for more information.

### Factory

If you wish to not use workers at all, use the factory packages. This will let you instantiate Porcupine engine instances directly.

The audio passed to the engine in the `process` function must be of the correct format. The `WebVoiceProcessor` handles downsampling in the examples above to standard voice recognition format (16-bit, 16kHz linear PCM, single-channel). Use an `Int16Array` [typed array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Typed_arrays). If you are not using `WebVoiceProcessor`, you must ensure the audio passed to Porcupine is of that format. The Porcupine instance provides the length of the array required via `.frameLength`.

E.g.:

```javascript
import { Porcupine } from "@picovoice/porcupine-web-en-factory";

async function startPorcupine() {
  const accessKey = // .. AccessKey string obtained from Picovoice Console (https://picovoice.ai/console/)
  const handle = await Porcupine.create(
    accessKey,
    [{builtin: "Bumblebee", sensitivity: 0.7}]);

  // Send Porcupine frames of audio (check handle.frameLength for size of array)
  const audioFrames = new Int16Array(/* Provide data with correct format and size */)
  const porcupineResult = handle.process(audioFrames)
  // porcupineResult: -1 if no detection occurred, otherwise returns the index of the item in the array 
  // (0 in this case for "Bumblebee")
}

startPorcupine()
```

**Important Note**: Because the factories are all-in-one packages that run an entire machine learning inference model in WebAssembly, they are approximately 1-2 MB in size. While this is tiny for a speech recognition, it's nontrivial for web delivery. Because of this, you likely will want to use dynamic `import()` instead of static `import {}` to reduce your app's starting bundle size. See e.g. https://webpack.js.org/guides/code-splitting/ for more information.

## Build from source (IIFE + ESM outputs)

This library uses Rollup and TypeScript along with Babel and other popular rollup plugins. There are two outputs: an IIFE version intended for script tags / CDN usage, and a JavaScript module version intended for use with modern JavaScript/TypeScript development (e.g. Angular, Create React App, Webpack).

```console
yarn
yarn build
```

The output will appear in the ./dist/ folder.
