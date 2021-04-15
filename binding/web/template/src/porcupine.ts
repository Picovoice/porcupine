/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import { PorcupineKeyword, PorcupineEngine } from './porcupine_types';

// @ts-ignore
import PorcupineEmscriptenModule from './lang/pv_porcupine_b64';

import {
  BuiltInKeyword,
  BUILT_IN_KEYWORD_BYTES,
} from './lang/built_in_keywords';

const DEFAULT_SENSITIVITY = 0.5;

type EmptyPromise = (value: void) => void;

/**
 * JavaScript/WebAssembly Binding for the Picovoice Porcupine wake word engine.
 *
 * It initializes the WebAssembly module and exposes an async factory method `create` for creating
 * new instances of the engine.
 *
 * The instances have JavaScript bindings that wrap the calls to the C library and
 * do some rudimentary type checking and parameter validation.
 */
class Porcupine implements PorcupineEngine {
  public static _frameLength = null;
  public static _initWasm = null;
  public static _processWasm = null;
  public static _releaseWasm = null;
  public static _sampleRate = null;
  public static _version = null;
  public static _porcupineModule = null;
  public static _emscriptenPromise: Promise<any> = PorcupineEmscriptenModule(
    {}
  );
  public static _wasmReadyResolve: EmptyPromise;
  public static _wasmReadyReject: EmptyPromise;
  private static _wasmPromise: Promise<void> = new Promise(
    (resolve, reject) => {
      Porcupine._wasmReadyResolve = resolve;
      Porcupine._wasmReadyReject = reject;
    }
  );

  private _handleWasm: number;
  private _pcmWasmPointer: number;
  private _keywordLabels: Map<number, string>;

  private constructor(
    handleWasm: number,
    pcmWasmPointer: number,
    keywordLabels: ArrayLike<string>
  ) {
    this._handleWasm = handleWasm;
    this._pcmWasmPointer = pcmWasmPointer;

    this._keywordLabels = new Map();
    for (let i = 0; i < keywordLabels.length; i++) {
      this._keywordLabels.set(i, keywordLabels[i]);
    }
  }

  /**
   * Releases resources acquired by WebAssembly module.
   */
  public release(): void {
    Porcupine._releaseWasm(this._handleWasm);
    Porcupine._porcupineModule._free(this._pcmWasmPointer);
  }

  /**
   * Processes a frame of audio. The required sample rate can be retrieved from '.sampleRate' and the length
   * of frame (number of audio samples per frame) can be retrieved from '.frameLength'. The audio needs to be
   * 16-bit linearly-encoded. Furthermore, the engine operates on single-channel audio.
   *
   * @param pcm - A frame of audio with properties described above.
   * @returns - Index of detected keyword (phrase). When no keyword is detected, it returns -1.
   */
  process(pcm: Int16Array): number {
    if (!(pcm instanceof Int16Array)) {
      throw new Error("The argument 'pcm' must be provided as an Int16Array")
    }

    const pcmWasmBuffer = new Uint8Array(
      Porcupine._porcupineModule.HEAPU8.buffer,
      this._pcmWasmPointer,
      pcm.byteLength
    );
    pcmWasmBuffer.set(new Uint8Array(pcm.buffer));

    const keywordIndex = Porcupine._processWasm(
      this._handleWasm,
      this._pcmWasmPointer
    );
    if (keywordIndex === -2) {
      throw new Error('Porcupine failed to process audio');
    }

    return keywordIndex;
  }

  get version(): string {
    return Porcupine._version;
  }

  get sampleRate(): number {
    return Porcupine._sampleRate;
  }

  get frameLength(): number {
    return Porcupine._frameLength;
  }

  get keywordLabels(): Map<number, string> {
    return this._keywordLabels;
  }

  /**
   * Creates an instance of the Porcupine wake word engine.
   * Behind the scenes, it requires the WebAssembly code to load and initialize before
   * it can create an instance.
   *
   * @param keywords - Built-in or Base64
   * representations of keywords and their sensitivities.
   * Can be provided as an array or a single keyword.
   *
   * @returns An instance of the Porcupine engine.
   */
  public static async create(
    keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string
  ): Promise<Porcupine> {
    // WASM initialization is asynchronous: until Emscripten is done loading,
    // then we will be able to create an instance of Porcupine.
    await Porcupine._wasmPromise;

    if (keywords === undefined || keywords === null) {
      throw new Error(
        'The keywords argument is undefined / empty; What would you like Porcupine to listen for?'
      );
    }

    if (!Array.isArray(keywords)) {
      keywords = [keywords];
    } else if (keywords.length === 0) {
      throw new Error(
        'The keywords argument array is empty; What would you like Porcupine to listen for?'
      );
    }

    const keywordSensitivities = [];
    const keywordModels = [];
    const keywordLabels = [];

    // Convert all PorcupineKeyword arguments to an array of bytes and an array of sensitivities
    for (const keyword of keywords) {
      const keywordArg = keyword;
      let keywordArgNormalized: PorcupineKeyword;

      // Convert string arguments to PorcupineKeyword object arguments
      // We need to infer what type of argument this is, because strings are ambiguous
      if (typeof keywordArg === 'string') {
        keywordArgNormalized = {
          builtin: keywordArg as BuiltInKeyword,
          sensitivity: DEFAULT_SENSITIVITY,
        };
      } else if (typeof keywordArg !== 'object') {
        throw new Error(
          'Invalid keyword argument type: ' +
          keywordArg +
          ' : ' +
          typeof keywordArg
        );
      } else {
        keywordArgNormalized = keywordArg;
      }

      if ('custom' in keywordArgNormalized) {
        // Custom keyword: Base64 string and label have been passed in
        keywordModels.push(
          Uint8Array.from(atob(keywordArgNormalized.base64), c =>
            c.charCodeAt(0)
          )
        );
        keywordLabels.push(keywordArgNormalized.custom);
      } else if ('builtin' in keywordArgNormalized) {
        // Built-in keyword: Look up the bytes from the map and convert
        const validEnums = Object.values(BuiltInKeyword);
        const builtInName = keywordArgNormalized.builtin;
        const keywordEnum = BuiltInKeyword[builtInName.replace(' ', '')];
        if (!validEnums.includes(keywordEnum)) {
          throw new Error(
            `Keyword ${builtInName} does not map to list of built-in keywords (${validEnums})`
          );
        }
        keywordModels.push(
          Uint8Array.from(atob(BUILT_IN_KEYWORD_BYTES.get(keywordEnum)), c =>
            c.charCodeAt(0)
          )
        );
        keywordLabels.push(keywordArgNormalized.builtin);
      } else {
        throw new Error(
          'Unknown keyword argument: ' + JSON.stringify(keywordArg)
        );
      }

      keywordSensitivities.push(
        keywordArgNormalized.sensitivity ?? DEFAULT_SENSITIVITY
      );
    }

    for (const sensitivity of keywordSensitivities) {
      if (typeof sensitivity !== "number") {
        throw new Error('Sensitivity is not a number (in range [0,1]): ' + sensitivity)
      }
      if (sensitivity < 0 || sensitivity > 1) {
        throw new Error('Sensitivity is outside of range [0, 1]: ' + sensitivity)
      }
    }

    if (keywordSensitivities.length !== keywordModels.length) {
      throw new Error(`keywordSensitivities (${keywordSensitivities.length}) and keywordModels (${keywordModels.length}) length differs`)
    }

    const sensitivities = new Float32Array(keywordSensitivities);

    const keywordModelSizes = Int32Array.from(
      keywordModels.map(x => x.byteLength)
    );
    const keywordModelSizesPointer = Porcupine._porcupineModule._malloc(
      keywordModelSizes.byteLength
    );
    const keywordModelSizesBuffer = new Uint8Array(
      Porcupine._porcupineModule.HEAPU8.buffer,
      keywordModelSizesPointer,
      keywordModelSizes.byteLength
    );
    keywordModelSizesBuffer.set(new Uint8Array(keywordModelSizes.buffer));

    const keywordModelPointers = Uint32Array.from(
      keywordModels.map(keywordModel => {
        const heapPointer = Porcupine._porcupineModule._malloc(
          keywordModel.byteLength
        );
        const heapBuffer = new Uint8Array(
          Porcupine._porcupineModule.HEAPU8.buffer,
          heapPointer,
          keywordModel.byteLength
        );
        heapBuffer.set(keywordModel);
        return heapPointer;
      })
    );

    const keywordModelPointersPointer = Porcupine._porcupineModule._malloc(
      keywordModelPointers.byteLength
    );
    const keywordModelPointersBuffer = new Uint8Array(
      Porcupine._porcupineModule.HEAPU8.buffer,
      keywordModelPointersPointer,
      keywordModelPointers.byteLength
    );
    keywordModelPointersBuffer.set(new Uint8Array(keywordModelPointers.buffer));

    const sensitivitiesPointer = Porcupine._porcupineModule._malloc(
      sensitivities.byteLength
    );
    const sensitivitiesBuffer = new Uint8Array(
      Porcupine._porcupineModule.HEAPU8.buffer,
      sensitivitiesPointer,
      sensitivities.byteLength
    );
    sensitivitiesBuffer.set(new Uint8Array(sensitivities.buffer));

    const handleWasm = Porcupine._initWasm(
      keywordModels.length,
      keywordModelSizesPointer,
      keywordModelPointersPointer,
      sensitivitiesPointer
    );
    if (handleWasm === 0) {
      throw new Error('Failed to initialize Porcupine._porcupineModule.');
    }

    const pcmWasmPointer = Porcupine._porcupineModule._malloc(
      2 * Porcupine._frameLength
    );

    return new Porcupine(handleWasm, pcmWasmPointer, keywordLabels);
  }
}

// Emscripten has fully loaded and initialized its WebAssembly module
Porcupine._emscriptenPromise.then(function (Module: any): void {
  Porcupine._initWasm = Module.cwrap('pv_porcupine_wasm_init', 'number', [
    'number',
    'number',
    'number',
    'number',
  ]);
  Porcupine._releaseWasm = Module.cwrap('pv_porcupine_wasm_delete', ['number']);
  Porcupine._processWasm = Module.cwrap('pv_porcupine_wasm_process', 'number', [
    'number',
    'number',
  ]);
  Porcupine._sampleRate = Module.cwrap('pv_wasm_sample_rate', 'number', [])();
  Porcupine._frameLength = Module.cwrap(
    'pv_porcupine_wasm_frame_length',
    'number',
    []
  )();
  Porcupine._version = Module.cwrap(
    'pv_porcupine_wasm_version',
    'string',
    []
  )();

  Porcupine._porcupineModule = Module;

  // Porcupine is ready to create instances!
  Porcupine._wasmReadyResolve();
});

export default Porcupine;
