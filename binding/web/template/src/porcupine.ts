/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

/* eslint camelcase: 0 */

// @ts-ignore
import * as Asyncify from 'asyncify-wasm';
import { Mutex } from 'async-mutex';

import { PorcupineKeyword, PorcupineEngine } from './porcupine_types';

// @ts-ignore
import { PORCUPINE_WASM_BASE64 } from './lang/porcupine_b64';
import { wasiSnapshotPreview1Emulator } from './wasi_snapshot';

import {
  BuiltInKeyword,
  BUILT_IN_KEYWORD_BYTES,
} from './lang/built_in_keywords';

import {
  arrayBufferToBase64AtIndex,
  arrayBufferToStringAtIndex,
  base64ToUint8Array,
  fetchWithTimeout,
  getPvStorage,
  isAccessKeyValid,
  stringHeaderToObject,
} from './utils';

const DEFAULT_SENSITIVITY = 0.5;
const PV_STATUS_SUCCESS = 10000;

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

type PorcupineWasmOutput = {
  frameLength: number;
  inputBufferAddress: number;
  memory: WebAssembly.Memory;
  objectAddress: number;
  pvPorcupineDelete: CallableFunction;
  pvPorcupineProcess: CallableFunction;
  pvStatusToString: CallableFunction;
  sampleRate: number;
  version: string;
  keywordIndexAddress: number;
};

export class Porcupine implements PorcupineEngine {
  private _pvPorcupineDelete: CallableFunction;
  private _pvPorcupineProcess: CallableFunction;
  private _pvStatusToString: CallableFunction;

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
    const memory = new WebAssembly.Memory({ initial: 1000, maximum: 2000 });

    const memoryBufferUint8 = new Uint8Array(memory.buffer);
    const memoryBufferInt32 = new Int32Array(memory.buffer);
    const memoryBufferFloat32 = new Float32Array(memory.buffer);

    const storage = getPvStorage();
    
    const pvConsoleLogWasm = function (index: number): void {
      // eslint-disable-next-line no-console
      console.log(arrayBufferToStringAtIndex(memoryBufferUint8, index));
    };
  
    const pvAssertWasm = function (
      expr: number,
      line: number,
      fileNameAddress: number
    ): void {
      if (expr === 0) {
        const fileName = arrayBufferToStringAtIndex(
          memoryBufferUint8,
          fileNameAddress
        );
        throw new Error(`assertion failed at line ${line} in "${fileName}"`);
      }
    };
  
    const pvTimeWasm = function (): number {
      return Date.now() / 1000;
    };
  
    const pvHttpsRequestWasm = async function (
      httpMethodAddress: number,
      serverNameAddress: number,
      endpointAddress: number,
      headerAddress: number,
      bodyAddress: number,
      timeoutMs: number,
      responseAddressAddress: number,
      responseSizeAddress: number,
      responseCodeAddress: number
    ): Promise<void> {
      const httpMethod = arrayBufferToStringAtIndex(
        memoryBufferUint8,
        httpMethodAddress
      );
      const serverName = arrayBufferToStringAtIndex(
        memoryBufferUint8,
        serverNameAddress
      );
      const endpoint = arrayBufferToStringAtIndex(
        memoryBufferUint8,
        endpointAddress
      );
      const header = arrayBufferToStringAtIndex(
        memoryBufferUint8,
        headerAddress
      );
      const body = arrayBufferToStringAtIndex(memoryBufferUint8, bodyAddress);
  
      const headerObject = stringHeaderToObject(header);
  
      let response: Response;
      let responseText: string;
      let statusCode: number;
  
      try {
        response = await fetchWithTimeout(
          'https://' + serverName + endpoint,
          {
            method: httpMethod,
            headers: headerObject,
            body: body,
          },
          timeoutMs
        );
        statusCode = response.status;
      } catch (error) {
        statusCode = 0;
      }
      // @ts-ignore
      if (response !== undefined) {
        try {
          responseText = await response.text();
        } catch (error) {
          responseText = '';
          statusCode = 1;
        }
        // eslint-disable-next-line
        const responseAddress = await aligned_alloc(
          Int8Array.BYTES_PER_ELEMENT,
          (responseText.length + 1) * Int8Array.BYTES_PER_ELEMENT
        );
        if (responseAddress === 0) {
          throw new Error('malloc failed: Cannot allocate memory');
        }
  
        memoryBufferInt32[
          responseSizeAddress / Int32Array.BYTES_PER_ELEMENT
        ] = responseText.length + 1;
        memoryBufferInt32[
          responseAddressAddress / Int32Array.BYTES_PER_ELEMENT
        ] = responseAddress;
  
        for (let i = 0; i < responseText.length; i++) {
          memoryBufferUint8[responseAddress + i] = responseText.charCodeAt(i);
        }
        memoryBufferUint8[responseAddress + responseText.length] = 0;
      }
  
      memoryBufferInt32[
        responseCodeAddress / Int32Array.BYTES_PER_ELEMENT
      ] = statusCode;
    };
  
    const pvFileLoadWasm = async function (
      pathAddress: number,
      numContentBytesAddress: number,
      contentAddressAddress: number,
      succeededAddress: number
    ): Promise<void> {
      const path = arrayBufferToStringAtIndex(memoryBufferUint8, pathAddress);
      try {
        const contentBase64 = await storage.getItem(path);
        const contentBuffer = base64ToUint8Array(contentBase64);
        // eslint-disable-next-line
        const contentAddress = await aligned_alloc(
          Uint8Array.BYTES_PER_ELEMENT,
          contentBuffer.length * Uint8Array.BYTES_PER_ELEMENT
        );
  
        if (contentAddress === 0) {
          throw new Error('malloc failed: Cannot allocate memory');
        }
  
        memoryBufferInt32[
          numContentBytesAddress / Int32Array.BYTES_PER_ELEMENT
        ] = contentBuffer.byteLength;
        memoryBufferInt32[
          contentAddressAddress / Int32Array.BYTES_PER_ELEMENT
        ] = contentAddress;
        memoryBufferUint8.set(contentBuffer, contentAddress);
        memoryBufferInt32[
          succeededAddress / Int32Array.BYTES_PER_ELEMENT
        ] = 1;
      } catch (error) {
        memoryBufferInt32[
          succeededAddress / Int32Array.BYTES_PER_ELEMENT
        ] = 0;
      }
    };
  
    const pvFileSaveWasm = async function (
      pathAddress: number,
      numContentBytes: number,
      contentAddress: number,
      succeededAddress: number
    ): Promise<void> {
      const path = arrayBufferToStringAtIndex(memoryBufferUint8, pathAddress);
      const content = arrayBufferToBase64AtIndex(
        memoryBufferUint8,
        numContentBytes,
        contentAddress
      );
      try {
        await storage.setItem(path, content);
        memoryBufferInt32[
          succeededAddress / Int32Array.BYTES_PER_ELEMENT
        ] = 1;
      } catch (error) {
        memoryBufferInt32[
          succeededAddress / Int32Array.BYTES_PER_ELEMENT
        ] = 0;
      }
    };
  
    const pvFileExistsWasm = async function (
      pathAddress: number,
      isExistsAddress: number,
      succeededAddress: number
    ): Promise<void> {
      const path = arrayBufferToStringAtIndex(memoryBufferUint8, pathAddress);
  
      try {
        const isExists = await storage.getItem(path);
        memoryBufferUint8[isExistsAddress] = (isExists === undefined || isExists === null) ? 0 : 1;
        memoryBufferInt32[
          succeededAddress / Int32Array.BYTES_PER_ELEMENT
        ] = 1;
      } catch (error) {
        memoryBufferInt32[
          succeededAddress / Int32Array.BYTES_PER_ELEMENT
        ] = 0;
      }
    };
  
    const pvFileDeleteWasm = async function (
      pathAddress: number,
      succeededAddress: number
    ): Promise<void> {
      const path = arrayBufferToStringAtIndex(memoryBufferUint8, pathAddress);
      try {
        await storage.removeItem(path);
        memoryBufferInt32[
          succeededAddress / Int32Array.BYTES_PER_ELEMENT
        ] = 1;
      } catch (error) {
        memoryBufferInt32[
          succeededAddress / Int32Array.BYTES_PER_ELEMENT
        ] = 0;
      }
    };
  
    const pvGetBrowserInfo = async function (browserInfoAddressAddress: number): Promise<void> {
      const userAgent =
        navigator.userAgent !== undefined ? navigator.userAgent : 'unknown';
      // eslint-disable-next-line
      const browserInfoAddress = await aligned_alloc(
        Uint8Array.BYTES_PER_ELEMENT,
        (userAgent.length + 1) * Uint8Array.BYTES_PER_ELEMENT
      );
  
      if (browserInfoAddress === 0) {
        throw new Error('malloc failed: Cannot allocate memory');
      }
  
      memoryBufferInt32[
        browserInfoAddressAddress / Int32Array.BYTES_PER_ELEMENT
      ] = browserInfoAddress;
      for (let i = 0; i < userAgent.length; i++) {
        memoryBufferUint8[browserInfoAddress + i] = userAgent.charCodeAt(i);
      }
      memoryBufferUint8[browserInfoAddress + userAgent.length] = 0;
    };
  
    const pvGetOriginInfo = async function(originInfoAddressAddress: number): Promise<void> {
      const origin = self.origin ?? self.location.origin;
      // eslint-disable-next-line
      const originInfoAddress = await aligned_alloc(
        Uint8Array.BYTES_PER_ELEMENT,
        (origin.length + 1) * Uint8Array.BYTES_PER_ELEMENT
      );
  
      if (originInfoAddress === 0) {
        throw new Error('malloc failed: Cannot allocate memory');
      }
  
      memoryBufferInt32[
        originInfoAddressAddress / Int32Array.BYTES_PER_ELEMENT
      ] = originInfoAddress;
      for (let i = 0; i < origin.length; i++) {
        memoryBufferUint8[originInfoAddress + i] = origin.charCodeAt(i);
      }
      memoryBufferUint8[originInfoAddress + origin.length] = 0;
    };

    const importObject = {
      // eslint-disable-next-line camelcase
      wasi_snapshot_preview1: wasiSnapshotPreview1Emulator,
      env: {
        memory: memory,
        // eslint-disable-next-line camelcase
        pv_console_log_wasm: pvConsoleLogWasm,
        // eslint-disable-next-line camelcase
        pv_assert_wasm: pvAssertWasm,
        // eslint-disable-next-line camelcase
        pv_time_wasm: pvTimeWasm,
        // eslint-disable-next-line camelcase
        pv_https_request_wasm: pvHttpsRequestWasm,
        // eslint-disable-next-line camelcase
        pv_file_load_wasm: pvFileLoadWasm,
        // eslint-disable-next-line camelcase
        pv_file_save_wasm: pvFileSaveWasm,
        // eslint-disable-next-line camelcase
        pv_file_exists_wasm: pvFileExistsWasm,
        // eslint-disable-next-line camelcase
        pv_file_delete_wasm: pvFileDeleteWasm,
        // eslint-disable-next-line camelcase
        pv_get_browser_info: pvGetBrowserInfo,
        // eslint-disable-next-line camelcase
        pv_get_origin_info: pvGetOriginInfo,
      },
    };

    const wasmCodeArray = base64ToUint8Array(PORCUPINE_WASM_BASE64);
    const { instance } = await Asyncify.instantiate(
      wasmCodeArray,
      importObject
    );

    const aligned_alloc = instance.exports.aligned_alloc as CallableFunction;
    const pv_porcupine_version = instance.exports
      .pv_porcupine_version as CallableFunction;
    const pv_porcupine_frame_length = instance.exports
      .pv_porcupine_frame_length as CallableFunction;
    const pv_porcupine_process = instance.exports
      .pv_porcupine_process as CallableFunction;
    const pv_porcupine_delete = instance.exports
      .pv_porcupine_delete as CallableFunction;
    const pv_porcupine_init = instance.exports.pv_porcupine_init as CallableFunction;
    const pv_status_to_string = instance.exports
      .pv_status_to_string as CallableFunction;
    const pv_sample_rate = instance.exports.pv_sample_rate as CallableFunction;
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
