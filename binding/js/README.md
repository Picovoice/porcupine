# Prerequisites

The binding assumes that the Porcupine's [WebAssembly library](/lib/wasm) is already loaded. 

# Binding Class

Create a new instance of engine using

```javascript
let obj = Porcupine.create(keywordIDs, sensitivities)
```

`keywordIDs` is an array of keyword signatures to be detected. Each signature is an array of 8-bit unsigned integers
(i.e. `UInt8Array`). A number of signatures are available for free under [resource folder](/resources/keyword_files) and
also within [demo file](/demo/js/voiceControlDemo.js).

`sensitivities` is an array of 32-bit floating point numbers (i.e. `Float32Array`) each value is the detection sensitivity
for each keyword to be detected. A higher sensitivity results in lower miss rate at the cost of higher false alarm rate.
Sensitivity values are within `[0, 1]`. For more details you can refer to the comments in
[C library's header file](/include/pv_porcupine.h).

When instantiated `Porcupine` can process audio via its `.process` method. When done be sure to release resources acquired
by WebAssembly using `.release`.

```javascript
    While (true) {
        let keywordIndex obj.process(audioFrameInt16Array);
    }
    
    obj.release();
```

`audioFrameInt16Array` is an array of audio samples in 16-bit format. The length of array can be retrieved using `obj.frameLength`
and the required sample rate using `obj.sampleRate`.

For a working example refer to [demo](/demo/js).