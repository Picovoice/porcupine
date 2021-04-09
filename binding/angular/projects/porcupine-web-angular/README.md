# porcupine-web-angular

Angular service for Porcupine for Web.

## Porcupine

Porcupine is a highly accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications.

> Porcupine wake word models for all major voice assistants (`Alexa`, `Hey Google`, `Ok Google`, and `Hey Siri`) are
> available for free (under Apache 2.0)

## Introduction

The Porcupine SDK for Angular is based on the Porcupine SDK for Web. The library provides an Angular service called `PorcupineService`. The package will take care of microphone access and audio downsampling (via `@picovoice/web-voice-processor`) and provide a wake word detection event to which your application can subscribe.

## Compatibility

The Picovoice SDKs for Web are powered by WebAssembly (WASM), the Web Audio API, and Web Workers.

All modern browsers (Chrome/Edge/Opera, Firefox, Safari) are supported, including on mobile. Internet Explorer is _not_ supported.

Using the Web Audio API requires a secure context (HTTPS connection), with the exception of `localhost`, for local development.

## Installation

Use `npm` or `yarn` to install the package and its peer dependencies. Each spoken language (e.g. 'en', 'de') is a separate package. For this example we'll use English:

```console
yarn add @picovoice/porcupine-web-angular @picovoice/web-voice-processor @picovoice/porcupine-web-en-worker
```

(or)

```console
npm install @picovoice/porcupine-web-angular @picovoice/web-voice-processor @picovoice/porcupine-web-en-worker
```

## Usage

In your Angular component, add the `PorcupineService`. The `PorcupineService` has a detection event to which you can subscribe:

```typescript
import { Subscription } from "rxjs"
import { PorcupineService } from "@picovoice/porcupine-web-angular"

...

  constructor(private porcupineService: PorcupineService) {
    // Subscribe to Porcupine Keyword detections
    // Store each detection so we can display it in an HTML list
    this.porcupineDetection = porcupineService.detection$.subscribe(
      keywordLabel => console.log(`Porcupine Detected "${keywordLabel}"`))
  }
```

We need to initialize Porcupine to tell it which keywords we want to listen to (and at what sensitivity). We can use the Angular lifecycle hooks `ngOnInit` and `ngOnDestroy` to start up and later tear down the Porcupine engine.

**Important Note** The @picovoice/porcupine-web-${LANGUAGE}-\* series of packages are on the order of ~1-2MB, as they contain the entire Voice AI model. Typically, you do _not_ want to import these statically, as your application bundle will be much larger than recommended. Instead, use dynamic imports so that the chunk is lazy-loaded:

```typescript
  async ngOnInit() {
    // Load Porcupine worker chunk with specific language model (large ~1-2MB chunk; dynamically imported)
    const porcupineFactoryEn = (await import('@picovoice/porcupine-web-en-worker')).PorcupineWorkerFactory
    // Initialize Porcupine Service
    try {
      await this.porcupineService.init(porcupineFactoryEn,
      {keywords: [{ builtin: "Okay Google", sensitivity: 0.65 }, { builtin: "Picovoice" }]})
      console.log("Porcupine is now loaded and listening")
    }
    catch (error) {
      console.error(error)
    }
  }

  ngOnDestroy() {
    this.porcupineDetection.unsubscribe()
    this.porcupineService.release()
  }

```

Upon mounting, the component will request microphone permission from the user, instantiate the audio stream, start up an instance of Porcupine, and listen for both "Picovoice" and "Okay Google". When the words are detected, they will be logged to the console.

The "Okay Google" word is listening at a sensitivity of 0.65, whereas "Picovoice" is using the default (0.5). Sensitivity is a value in the range [0,1] which controls the tradeoff between miss rate and false alarm.

### Custom wake words

Each language includes a set of built-in keywords. The quickest way to get started is to use one of those. The builtin keywords are licensed under Apache-2.0 and are completely free to use.

Custom wake words are generated using [Picovoice Console](https://picovoice.ai/console/). They are trained from text using transfer learning into bespoke Porcupine keyword files with a `.ppn` extension. The target platform is WebAssembly (WASM), as that is what backs the Angular library.

Convert the `.ppn` file to base64 and provide it as an argument to Porcupine as below. You will need to also provide a label so that the `PorcupineService` can tell you which keyword occurred ("Deep Sky Blue", in this case):

```typescript
const DEEP_SKY_BLUE_PPN_64 = /* Base64 representation of deep_sky_blue.ppn */

...
  // Listen for "Deep Sky Blue": pass in a base64-encoded string of the .ppn file:
  await this.porcupineService.init(porcupineFactoryEn,
  {keywords: [{ custom: "Deep Sky Blue", base64: DEEP_SKY_BLUE_PPN_64 }]})

...
```

You may wish to store the base64 string in a separate JavaScript file and export it to keep your application code separate.
