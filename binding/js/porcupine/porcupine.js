let Porcupine = (function () {
    /**
     * Binding for wake word detection object. It initializes the JS binding for WebAssembly module and exposes
     * a factory method for creating new instances of wake word engine.
     */

    let initWasm = null;
    let releaseWasm = null;
    let processWasm = null;
    let sampleRate = null;
    let frameLength = null;
    let version = null;

    let porcupineModule = PorcupineModule();
    porcupineModule.then(function(Module) {
        initWasm = Module.cwrap('pv_porcupine_wasm_init', 'number', ['number', 'number', 'number', 'number']);
        releaseWasm = Module.cwrap('pv_porcupine_wasm_delete', ['number']);
        processWasm = Module.cwrap('pv_porcupine_wasm_process', 'number', ['number', 'number']);
        sampleRate = Module.cwrap('pv_wasm_sample_rate', 'number', [])();
        frameLength = Module.cwrap('pv_porcupine_wasm_frame_length', 'number', [])();
        version = Module.cwrap('pv_porcupine_wasm_version', 'string', [])();
    });

    let isLoaded = function() {
        /**
         * Flag indicating if 'PorcupineModule' is loaded. .create() can only be called after loading is finished.
         */
        return initWasm != null;
    };

    let create = function (keywordIDs, sensitivities) {
        /**
         * Creates an instance of wake word detection engine (aka porcupine). Can be called only after .isLoaded()
         * returns true.
         * @param {Array} Array of keyword IDs. A keyword ID is the signature for a given phrase to be detected. Each
         * keyword ID should be stored as UInt8Array.
         * @param {Float32Array} Detection sensitivity. A higher sensitivity reduces miss rate at the cost of higher
         * false alarm rate. Sensitivity is a number within [0, 1].
         * @returns An instance of wake word detection engine.
         */
        let keywordIDSizes = Int32Array.from(keywordIDs.map(keywordID => keywordID.byteLength));
        let keywordIDSizesPointer = porcupineModule._malloc(keywordIDSizes.byteLength);
        let keywordIDSizesBuffer = new Uint8Array(porcupineModule.HEAPU8.buffer, keywordIDSizesPointer, keywordIDSizes.byteLength);
        keywordIDSizesBuffer.set(new Uint8Array(keywordIDSizes.buffer));

        let keywordIDPointers = Uint32Array.from(keywordIDs.map(keywordID => {
            let heapPointer = porcupineModule._malloc(keywordID.byteLength);
            let heapBuffer = new Uint8Array(porcupineModule.HEAPU8.buffer, heapPointer, keywordID.byteLength);
            heapBuffer.set(keywordID);
            return heapPointer;
        }));

        let keywordIDPointersPointer = porcupineModule._malloc(keywordIDPointers.byteLength);
        let keywordIDPointersBuffer = new Uint8Array(
            porcupineModule.HEAPU8.buffer,
            keywordIDPointersPointer,
            keywordIDPointers.byteLength);
        keywordIDPointersBuffer.set(new Uint8Array(keywordIDPointers.buffer));

        let sensitivitiesPointer = porcupineModule._malloc(sensitivities.byteLength);
        let sensitivitiesBuffer = new Uint8Array(porcupineModule.HEAPU8.buffer, sensitivitiesPointer, sensitivities.byteLength);
        sensitivitiesBuffer.set(new Uint8Array(sensitivities.buffer));

        let handleWasm = initWasm(
            keywordIDs.length,
            keywordIDSizesPointer,
            keywordIDPointersPointer,
            sensitivitiesPointer);
        if (handleWasm === 0) {
            throw new Error("failed to initialize porcupine.");
        }

        let pcmWasmPointer = porcupineModule._malloc(frameLength * 2);

        let release = function () {
            /**
             * Releases resources acquired by WebAssembly module.
             */

            releaseWasm(handleWasm);
            porcupineModule._free(pcmWasmPointer);
        };

        let process = function (pcmInt16Array) {
            /**
             * Processes a frame of audio. The required sample rate can be retrieved from .sampleRate and the length of
             * frame (number of samples within frame) can be retrieved from .frameLength.
             * @param {Int16Array} A frame of audio with properties described above.
             * @returns {Number} Index of detected keyword (phrase). When no keyword is detected it returns -1.
             */

            let pcmWasmBuffer = new Uint8Array(porcupineModule.HEAPU8.buffer, pcmWasmPointer, pcmInt16Array.byteLength);
            pcmWasmBuffer.set(new Uint8Array(pcmInt16Array.buffer));

            let keyword_index = processWasm(handleWasm, pcmWasmPointer);
            if (keyword_index === -2) {
                throw new Error("porcupine failed to process audio");
            }

            return keyword_index;
        };

        return {
            release: release,
            process: process,
            sampleRate: sampleRate,
            frameLength: frameLength,
            version: version
        }
    };

    return {isLoaded: isLoaded, create: create}
})();
