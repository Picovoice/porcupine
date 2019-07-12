# PorcupineManager

This is a library to recognize wake words and voice commands in the browser.

- Private, as it runs locally within the browser and the voice data does not leave the device.
- Real-time with minimal latency, as it does not depend on network calls.
- Optimized using WebAssembly for algorithm performance.

## Compatibility

PorcupineManager uses the [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API) and
[WebAssembly](https://webassembly.org/), which are supported on all modern browsers (excluding Internet Explorer).

## Installation

```bash
npm install porcupine-manager
```

## Usage

Add the following to your HTML:

```html
<script src="{PATH_TO_WEB_VOICE_PROCESSOR_JS}"></script>
<script src="{PATH_TO_PORCUPINE_MANAGER_JS}"></script>
```
Replace `{PATH_TO_WEB_VOICE_PROCESSOR_JS}` with path to `src/web_voice_processor.js`, which is a dependency of
`porcupine_manager` and replace `{PATH_TO_PORCUPINE_MANAGER_JS}` with the path to
[src/porcupine_manager.js](src/porcupine_manager.js).

### Instantiation

The library adds a class to the global scope that can be used to instantiate an instance:

```javascript
let porcupineWorkerScript = ... // Path to porcupine_worker.js within the package
let downsamplingWorkerScript = ... // Path to downsampling_worker.js script within web-voice-processor package

porcupineManager = PorcupineManager(porcupineWorkerScript, downsamplingWorkerScript);
```

### Start Processing

Start the detection process:

```javascript
porcupineManager.start(keywordIDs, sensitivities, detectionCallback, errorCallback)
```

`keywordIDs` is an array of keyword signatures to be detected. Each signature is an array of 8-bit unsigned integers
(i.e. `UInt8Array`). A number of signatures are available for free under the [resource folder](/resources/keyword_files) and
also within the [demo file](/demo/js/voiceControlDemo.js).

`sensitivities` is an array of 32-bit floating point numbers (i.e. `Float32Array`); each value corresponds to the
detection sensitivity for each keyword. A higher sensitivity results in a lower miss rate, at the cost of a higher
false positive rate. Sensitivity values are within `[0, 1]`. For more details, you can refer to the comments in the
[C library's header file](/include/pv_porcupine.h).

`detectionCallback` is called after processing each frame.

```javascript
let detectionCallback = function (keyword) {
    // do something ...
};
```

If no keyword is detected, the input argument will be `null`; otherwise, it corresponds to detected keyword.

`errorCallback` is executed when there is an error in the audio capture or processing logic.

### Stop Processing

```javascript
porcupineManager.stop()
```
