/*
  Copyright 2018-2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

/* eslint camelcase: 0 */

// @ts-ignore
import { Mutex } from 'async-mutex';

import { PorcupineKeyword, PorcupineEngine } from '@picovoice/porcupine-web-core';

import { 
  aligned_alloc_type, 
  buildWasm,
  arrayBufferToStringAtIndex,
  isAccessKeyValid
} from '@picovoice/web-utils';

// @ts-ignore
import { PORCUPINE_WASM_BASE64 } from './lang/porcupine_b64';

import {
  BuiltInKeyword,
  BUILT_IN_KEYWORD_BYTES,
} from './lang/built_in_keywords';

const DEFAULT_SENSITIVITY = 0.5;
const PV_STATUS_SUCCESS = 10000;

/**
 * WebAssembly function types
 */

 type pv_porcupine_init_type = (
   accessKey: number, 
   numKeywords: number, 
   keywordModelSizes: number, 
   keywordModels: number, 
   sensitivies: number, 
   object: number
 ) => Promise<number>;
 type pv_porcupine_process_type = (object: number, buffer: number, keywordIndex: number) => Promise<number>;
 type pv_porcupine_delete_type = (object: number) => Promise<void>;
 type pv_status_to_string_type = (status: number) => Promise<number>
 type pv_sample_rate_type = () => Promise<number>;
 type pv_porcupine_frame_length_type = () => Promise<number>;
 type pv_porcupine_version_type = () => Promise<number>;

/**
 * JavaScript/WebAssembly Binding for the Picovoice Porcupine wake word engine.
 *
 * It initializes the WebAssembly module and exposes an async factory method `create` for creating
 * new instances of the engine.
 *
 * The instances have JavaScript bindings that wrap the calls to the C library and
 * do some rudimentary type checking and parameter validation.
 */

type PorcupineWasmOutput = {
  frameLength: number;
  inputBufferAddress: number;
  memory: WebAssembly.Memory;
  objectAddress: number;
  pvPorcupineDelete: pv_porcupine_delete_type;
  pvPorcupineProcess: pv_porcupine_process_type;
  pvStatusToString: pv_status_to_string_type;
  sampleRate: number;
  version: string;
  keywordIndexAddress: number;
};

export class Porcupine implements PorcupineEngine {
  private _pvPorcupineDelete: pv_porcupine_delete_type;
  private _pvPorcupineProcess: pv_porcupine_process_type;
  private _pvStatusToString: pv_status_to_string_type;

  private _wasmMemory: WebAssembly.Memory;
  private _memoryBuffer: Int16Array;
  private _memoryBufferView: DataView;
  private _processMutex: Mutex;

  private _objectAddress: number;
  private _inputBufferAddress: number;
  private _keywordIndexAddress: number;
  private _keywordLabels: Map<number, string>;

  private static _frameLength: number;
  private static _sampleRate: number;
  private static _version: string;

  private static _porcupineMutex = new Mutex;

  private constructor(handleWasm: PorcupineWasmOutput, keywordLabels: ArrayLike<string>) {
    Porcupine._frameLength = handleWasm.frameLength;
    Porcupine._sampleRate = handleWasm.sampleRate;
    Porcupine._version = handleWasm.version;

    this._pvPorcupineDelete = handleWasm.pvPorcupineDelete;
    this._pvPorcupineProcess = handleWasm.pvPorcupineProcess;
    this._pvStatusToString = handleWasm.pvStatusToString;

    this._wasmMemory = handleWasm.memory;
    this._objectAddress = handleWasm.objectAddress;
    this._inputBufferAddress = handleWasm.inputBufferAddress;
    this._keywordIndexAddress = handleWasm.keywordIndexAddress;

    this._memoryBuffer = new Int16Array(handleWasm.memory.buffer);
    this._memoryBufferView = new DataView(handleWasm.memory.buffer);

    this._keywordLabels = new Map();
    for (let i = 0; i < keywordLabels.length; i++) {
      this._keywordLabels.set(i, keywordLabels[i]);
    }
    this._processMutex = new Mutex();
  }

  /**
   * Releases resources acquired by WebAssembly module.
   */
  public async release(): Promise<void> {
    await this._pvPorcupineDelete(this._objectAddress);
  }

  /**
   * Processes a frame of audio. The required sample rate can be retrieved from '.sampleRate' and the length
   * of frame (number of audio samples per frame) can be retrieved from '.frameLength'. The audio needs to be
   * 16-bit linearly-encoded. Furthermore, the engine operates on single-channel audio.
   *
   * @param pcm - A frame of audio with properties described above.
   * @returns - Index of detected keyword (phrase). When no keyword is detected, it returns -1.
   */
  public async process(pcm: Int16Array): Promise<number> {
    if (!(pcm instanceof Int16Array)) {
      throw new Error("The argument 'pcm' must be provided as an Int16Array");
    }
    const returnPromise = new Promise<number>((resolve, reject) => {
      this._processMutex.runExclusive(async () => {
        this._memoryBuffer.set(
          pcm,
          this._inputBufferAddress / Int16Array.BYTES_PER_ELEMENT
        );
        const status = await this._pvPorcupineProcess(
          this._objectAddress,
          this._inputBufferAddress,
          this._keywordIndexAddress
        );
        if (status !== PV_STATUS_SUCCESS) {
          const memoryBuffer = new Uint8Array(this._wasmMemory.buffer);
          throw new Error(
            `process failed with status ${arrayBufferToStringAtIndex(
              memoryBuffer,
              await this._pvStatusToString(status)
            )}`
          );
        }
        return this._memoryBufferView.getInt32(
          this._keywordIndexAddress,
          true
        );
      }).then((result: number) => {
        resolve(result);
      }).catch((error: any) => {
        reject(error);
      });
    });
    return returnPromise;
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
   * @param accessKey - AccessKey obtained from Picovoice Console (https://picovoice.ai/console/)
   * @param keywords - Built-in or Base64
   * representations of keywords and their sensitivities.
   * Can be provided as an array or a single keyword.
   *
   * @returns An instance of the Porcupine engine.
   */

  public static async create(
    accessKey: string,
    keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string
  ): Promise<Porcupine> {
    if (!isAccessKeyValid(accessKey)) {
      throw new Error('Invalid AccessKey');
    }

    if (keywords === undefined || keywords === null) {
      throw new Error(
        'The keywords argument is undefined / empty; What would you like Porcupine to listen for?'
      );
    }

    const returnPromise = new Promise<Porcupine>((resolve, reject) => {
      Porcupine._porcupineMutex.runExclusive(async () => {
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

        const wasmOutput = await Porcupine.initWasm(
          accessKey,
          keywordModels,
          keywordModelSizes,
          sensitivities,
        );
        return new Porcupine(wasmOutput, keywordLabels);
      }).then((result: Porcupine) => {
        resolve(result);
      }).catch((error: any) => {
        reject(error);
      });
    });
    return returnPromise;
  }

  private static async initWasm(
    accessKey: string,
    keywordModels: ArrayLike<Uint8Array>,
    keywordModelSizes: Int32Array,
    sensitivities: Float32Array): Promise<any> {
    const memory = new WebAssembly.Memory({ initial: 10, maximum: 300 });

    const memoryBufferUint8 = new Uint8Array(memory.buffer);
    const memoryBufferInt32 = new Int32Array(memory.buffer);
    const memoryBufferFloat32 = new Float32Array(memory.buffer);

    const exports = await buildWasm(memory, PORCUPINE_WASM_BASE64);

    const aligned_alloc = exports.aligned_alloc as aligned_alloc_type;
    const pv_porcupine_version = exports.pv_porcupine_version as pv_porcupine_version_type;
    const pv_porcupine_frame_length = exports.pv_porcupine_frame_length as pv_porcupine_frame_length_type;
    const pv_porcupine_process = exports.pv_porcupine_process as pv_porcupine_process_type;
    const pv_porcupine_delete = exports.pv_porcupine_delete as pv_porcupine_delete_type;
    const pv_porcupine_init = exports.pv_porcupine_init as pv_porcupine_init_type;
    const pv_status_to_string = exports.pv_status_to_string as pv_status_to_string_type;
    const pv_sample_rate = exports.pv_sample_rate as pv_sample_rate_type;
    const keywordIndexAddress = await aligned_alloc(
      Int32Array.BYTES_PER_ELEMENT,
      Int32Array.BYTES_PER_ELEMENT
    );

    if (keywordIndexAddress === 0) {
      throw new Error('malloc failed: Cannot allocate memory');
    }
    const objectAddressAddress = await aligned_alloc(
      Int32Array.BYTES_PER_ELEMENT,
      Int32Array.BYTES_PER_ELEMENT
    );
    if (objectAddressAddress === 0) {
      throw new Error('malloc failed: Cannot allocate memory');
    }
    const accessKeyAddress = await aligned_alloc(
      Uint8Array.BYTES_PER_ELEMENT,
      (accessKey.length + 1) * Uint8Array.BYTES_PER_ELEMENT
    );

    if (accessKeyAddress === 0) {
      throw new Error('malloc failed: Cannot allocate memory');
    }

    for (let i = 0; i < accessKey.length; i++) {
      memoryBufferUint8[accessKeyAddress + i] = accessKey.charCodeAt(i);
    }
    memoryBufferUint8[accessKeyAddress + accessKey.length] = 0;

    const keywordModelSizeAddress = await aligned_alloc(
      Int32Array.BYTES_PER_ELEMENT,
      keywordModels.length * Int32Array.BYTES_PER_ELEMENT
    );
    if (keywordModelSizeAddress === 0) {
      throw new Error("malloc failed: Cannot allocate memory");
    }
    memoryBufferInt32.set(
      keywordModelSizes,
      keywordModelSizeAddress / Int32Array.BYTES_PER_ELEMENT
    );
    let keywordModelAddresses = [];
    for (let i = 0; i < keywordModels.length; i++) {
      keywordModelAddresses.push(
        await aligned_alloc(
          Int8Array.BYTES_PER_ELEMENT,
          keywordModelSizes[i] * Int8Array.BYTES_PER_ELEMENT
        )
      );
      if (keywordModelAddresses[i] === 0) {
        throw new Error("malloc failed: Cannot allocate memory");
      }
      memoryBufferUint8.set(keywordModels[i], keywordModelAddresses[i]);
    }
    const keywordModelAddressAddress = await aligned_alloc(
      Int32Array.BYTES_PER_ELEMENT,
      keywordModels.length * Int32Array.BYTES_PER_ELEMENT
    );
    if (keywordModelAddressAddress === 0) {
      throw new Error("malloc failed: Cannot allocate memory");
    }
    memoryBufferInt32.set(
      keywordModelAddresses,
      keywordModelAddressAddress / Int32Array.BYTES_PER_ELEMENT
    );

    const sensitivityAddress = await aligned_alloc(
      Float32Array.BYTES_PER_ELEMENT,
      keywordModels.length * Float32Array.BYTES_PER_ELEMENT
    );
    if (sensitivityAddress === 0) {
      throw new Error("malloc failed: Cannot allocate memory");
    }
    memoryBufferFloat32.set(
      sensitivities,
      sensitivityAddress / Float32Array.BYTES_PER_ELEMENT
    );
    const status = await pv_porcupine_init(
      accessKeyAddress,
      keywordModels.length,
      keywordModelSizeAddress,
      keywordModelAddressAddress,
      sensitivityAddress,
      objectAddressAddress);
    if (status !== PV_STATUS_SUCCESS) {
      throw new Error(
        `'pv_porcupine_init' failed with status ${arrayBufferToStringAtIndex(
          memoryBufferUint8,
          await pv_status_to_string(status)
        )}`
      );
    }
    const memoryBufferView = new DataView(memory.buffer);
    const objectAddress = memoryBufferView.getInt32(
      objectAddressAddress,
      true
    );

    const sampleRate = await pv_sample_rate();
    const frameLength = await pv_porcupine_frame_length();
    const versionAddress = await pv_porcupine_version();
    const version = arrayBufferToStringAtIndex(
      memoryBufferUint8,
      versionAddress
    );

    const inputBufferAddress = await aligned_alloc(
      Int16Array.BYTES_PER_ELEMENT,
      frameLength * Int16Array.BYTES_PER_ELEMENT
    );
    if (inputBufferAddress === 0) {
      throw new Error('malloc failed: Cannot allocate memory');
    }

    return {
      frameLength: frameLength,
      inputBufferAddress: inputBufferAddress,
      memory: memory,
      objectAddress: objectAddress,
      pvPorcupineDelete: pv_porcupine_delete,
      pvPorcupineProcess: pv_porcupine_process,
      pvStatusToString: pv_status_to_string,
      keywordIndexAddress: keywordIndexAddress,
      sampleRate: sampleRate,
      version: version,
    };
  }
}

export default Porcupine;
