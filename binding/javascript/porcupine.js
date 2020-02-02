/*
    Copyright 2018 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

let Porcupine = (function () {
    /**
     * Binding for wake word engine (Porcupine). It initializes the JavaScript binding for WebAssembly module and
     * exposes a factory method for creating new instances of the engine.
     */

    let initWasm = null;
    let releaseWasm = null;
    let processWasm = null;
    let sampleRate = null;
    let frameLength = null;
    let version = null;

    let porcupineModule = PorcupineModule();
    porcupineModule.then(function (Module) {
        initWasm = Module.cwrap('pv_porcupine_wasm_init', 'number', ['number', 'number', 'number', 'number']);
        releaseWasm = Module.cwrap('pv_porcupine_wasm_delete', ['number']);
        processWasm = Module.cwrap('pv_porcupine_wasm_process', 'number', ['number', 'number']);
        sampleRate = Module.cwrap('pv_wasm_sample_rate', 'number', [])();
        frameLength = Module.cwrap('pv_porcupine_wasm_frame_length', 'number', [])();
        version = Module.cwrap('pv_porcupine_wasm_version', 'string', [])();
    });

    let isLoaded = function () {
        /**
         * Flag indicating if 'PorcupineModule' is loaded. '.create()' can only be called after loading is finished.
         */

        return initWasm != null;
    };

    let create = function (keywordModels, sensitivities) {
        /**
         * Creates an instance of wake word engine (Porcupine). Can be called only after '.isLoaded()' returns true.
         * @param {Array} Array of keyword models. A keyword model is the signature for a given phrase to be detected.
         * Each keyword model should be stored as 'UInt8Array'.
         * @param {Float32Array} Detection sensitivities for each keywords model. A higher sensitivity reduces miss rate
         * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
         * [0, 1].
         * @returns An instance of the engine.
         */

        let keywordModelSizes = Int32Array.from(keywordModels.map(x => x.byteLength));
        let keywordModelSizesPointer = porcupineModule._malloc(keywordModelSizes.byteLength);
        let keywordModelSizesBuffer = new Uint8Array(
            porcupineModule.HEAPU8.buffer,
            keywordModelSizesPointer,
            keywordModelSizes.byteLength);
        keywordModelSizesBuffer.set(new Uint8Array(keywordModelSizes.buffer));

        let keywordModelPointers = Uint32Array.from(keywordModels.map(keywordModel => {
            let heapPointer = porcupineModule._malloc(keywordModel.byteLength);
            let heapBuffer = new Uint8Array(porcupineModule.HEAPU8.buffer, heapPointer, keywordModel.byteLength);
            heapBuffer.set(keywordModel);
            return heapPointer;
        }));

        let keywordModelPointersPointer = porcupineModule._malloc(keywordModelPointers.byteLength);
        let keywordModelPointersBuffer = new Uint8Array(
            porcupineModule.HEAPU8.buffer,
            keywordModelPointersPointer,
            keywordModelPointers.byteLength);
        keywordModelPointersBuffer.set(new Uint8Array(keywordModelPointers.buffer));

        let sensitivitiesPointer = porcupineModule._malloc(sensitivities.byteLength);
        let sensitivitiesBuffer = new Uint8Array(
            porcupineModule.HEAPU8.buffer,
            sensitivitiesPointer,
            sensitivities.byteLength);
        sensitivitiesBuffer.set(new Uint8Array(sensitivities.buffer));

        let handleWasm = initWasm(
            keywordModels.length,
            keywordModelSizesPointer,
            keywordModelPointersPointer,
            sensitivitiesPointer);
        if (handleWasm === 0) {
            throw new Error("failed to initialize Porcupine.");
        }

        let pcmWasmPointer = porcupineModule._malloc(2 * frameLength);

        let release = function () {
            /**
             * Releases resources acquired by WebAssembly module.
             */

            releaseWasm(handleWasm);
            porcupineModule._free(pcmWasmPointer);
        };

        let process = function (pcmInt16Array) {
            /**
             * Processes a frame of audio. The required sample rate can be retrieved from '.sampleRate' and the length
             * of frame (number of audio samples per frame) can be retrieved from '.frameLength'. The audio needs to be
             * 16-bit linearly-encoded. Furthermore, the engine operates on single-channel audio.
             * @param {Int16Array} A frame of audio with properties described above.
             * @returns {Number} Index of detected keyword (phrase). When no keyword is detected it returns -1.
             */

            let pcmWasmBuffer = new Uint8Array(porcupineModule.HEAPU8.buffer, pcmWasmPointer, pcmInt16Array.byteLength);
            pcmWasmBuffer.set(new Uint8Array(pcmInt16Array.buffer));

            let keyword_index = processWasm(handleWasm, pcmWasmPointer);
            if (keyword_index === -2) {
                throw new Error("Porcupine failed to process audio");
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
