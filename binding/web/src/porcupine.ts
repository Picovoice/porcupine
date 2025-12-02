/*
  Copyright 2022-2025 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

/* eslint camelcase: 0 */

import { Mutex } from 'async-mutex';

import {
  arrayBufferToStringAtIndex,
  base64ToUint8Array,
  isAccessKeyValid,
  loadModel,
} from '@picovoice/web-utils';

import createModuleSimd from "./lib/pv_porcupine_simd";
import createModulePThread from "./lib/pv_porcupine_pthread";

import { simd } from 'wasm-feature-detect';

import {
  DetectionCallback,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PvStatus
} from './types';

import { keywordsProcess } from './utils';
import { BuiltInKeyword } from './built_in_keywords';
import * as PorcupineErrors from "./porcupine_errors";
import { pvStatusToException } from './porcupine_errors';

/**
 * WebAssembly function types
 */

type pv_porcupine_init_type = (
  accessKey: number,
  modelPath: number,
  device: number,
  numKeywords: number,
  keywordPaths: number,
  sensitivities: number,
  object: number
) => Promise<number>;
type pv_porcupine_process_type = (
  object: number,
  pcm: number,
  keywordIndex: number
) => number;
type pv_porcupine_delete_type = (object: number) => void;
type pv_sample_rate_type = () => number;
type pv_porcupine_frame_length_type = () => number;
type pv_porcupine_version_type = () => number;
type pv_porcupine_list_hardware_devices_type = (
  hardwareDevices: number,
  numHardwareDevices: number
) => number;
type pv_porcupine_free_hardware_devices_type = (
  hardwareDevices: number,
  numHardwareDevices: number
) => number;
type pv_set_sdk_type = (sdk: number) => void;
type pv_get_error_stack_type = (messageStack: number, messageStackDepth: number) => number;
type pv_free_error_stack_type = (messageStack: number) => void;

/**
 * JavaScript/WebAssembly Binding for the Picovoice Porcupine wake word engine.
 *
 * The instances have JavaScript bindings that wrap the calls to the C library and
 * do some rudimentary type checking and parameter validation.
 */

type PorcupineModule = EmscriptenModule & {
  _pv_free: (address: number) => void;

  _pv_porcupine_delete: pv_porcupine_delete_type;
  _pv_porcupine_process: pv_porcupine_process_type;
  _pv_porcupine_frame_length: pv_porcupine_frame_length_type
  _pv_porcupine_version: pv_porcupine_version_type
  _pv_porcupine_list_hardware_devices: pv_porcupine_list_hardware_devices_type;
  _pv_porcupine_free_hardware_devices: pv_porcupine_free_hardware_devices_type;
  _pv_sample_rate: pv_sample_rate_type

  _pv_set_sdk: pv_set_sdk_type;
  _pv_get_error_stack: pv_get_error_stack_type;
  _pv_free_error_stack: pv_free_error_stack_type;

  // em default functions
  addFunction: typeof addFunction;
  ccall: typeof ccall;
  cwrap: typeof cwrap;
}

type PorcupineWasmOutput = {
  module: PorcupineModule;

  frameLength: number;
  sampleRate: number;
  version: string;

  objectAddress: number;
  inputBufferAddress: number;
  keywordIndexAddress: number;
  messageStackAddressAddressAddress: number;
  messageStackDepthAddress: number;
};

export class Porcupine {
  private _module?: PorcupineModule;

  private readonly _frameLength: number;
  private readonly _sampleRate: number;
  private readonly _version: string;

  private readonly _processMutex: Mutex;

  private readonly _objectAddress: number;
  private readonly _inputBufferAddress: number;
  private readonly _keywordIndexAddress: number;
  private readonly _keywordLabels: Map<number, string>;
  private readonly _messageStackAddressAddressAddress: number;
  private readonly _messageStackDepthAddress: number;

  private static _wasmSimd: string;
  private static _wasmSimdLib: string;
  private static _wasmPThread: string;
  private static _wasmPThreadLib: string;

  private static _sdk: string = "web";

  private static _porcupineMutex = new Mutex();

  private readonly _keywordDetectionCallback: DetectionCallback;
  private readonly _processErrorCallback?: (error: PorcupineErrors.PorcupineError) => void;

  private constructor(
    handleWasm: PorcupineWasmOutput,
    keywordLabels: ArrayLike<string>,
    keywordDetectionCallback: DetectionCallback,
    processErrorCallback?: (error: PorcupineErrors.PorcupineError) => void
  ) {
    this._module = handleWasm.module;

    this._frameLength = handleWasm.frameLength;
    this._sampleRate = handleWasm.sampleRate;
    this._version = handleWasm.version;

    this._objectAddress = handleWasm.objectAddress;
    this._inputBufferAddress = handleWasm.inputBufferAddress;
    this._keywordIndexAddress = handleWasm.keywordIndexAddress;
    this._messageStackAddressAddressAddress = handleWasm.messageStackAddressAddressAddress;
    this._messageStackDepthAddress = handleWasm.messageStackDepthAddress;

    this._keywordLabels = new Map();
    for (let i = 0; i < keywordLabels.length; i++) {
      this._keywordLabels.set(i, keywordLabels[i]);
    }

    this._processMutex = new Mutex();

    this._keywordDetectionCallback = keywordDetectionCallback;
    this._processErrorCallback = processErrorCallback;
  }

  /**
   * Get Porcupine engine version.
   */
  get version(): string {
    return this._version;
  }

  /**
   * Get frame length.
   */
  get frameLength(): number {
    return this._frameLength;
  }

  /**
   * Get sample rate.
   */
  get sampleRate(): number {
    return this._sampleRate;
  }

  /**
   * Get keyword labels.
   */
  get keywordLabels(): Map<number, string> {
    return this._keywordLabels;
  }

  /**
   * Set base64 wasm file with SIMD feature.
   * @param wasmSimd Base64'd wasm file to use to initialize wasm.
   */
  public static setWasmSimd(wasmSimd: string): void {
    if (this._wasmSimd === undefined) {
      this._wasmSimd = wasmSimd;
    }
  }

  /**
   * Set base64 SIMD wasm file in text format.
   * @param wasmSimdLib Base64'd wasm file in text format.
   */
  public static setWasmSimdLib(wasmSimdLib: string): void {
    if (this._wasmSimdLib === undefined) {
      this._wasmSimdLib = wasmSimdLib;
    }
  }

  /**
   * Set base64 wasm file with SIMD and pthread feature.
   * @param wasmPThread Base64'd wasm file to use to initialize wasm.
   */
  public static setWasmPThread(wasmPThread: string): void {
    if (this._wasmPThread === undefined) {
      this._wasmPThread = wasmPThread;
    }
  }

  /**
   * Set base64 SIMD and thread wasm file in text format.
   * @param wasmPThreadLib Base64'd wasm file in text format.
   */
  public static setWasmPThreadLib(wasmPThreadLib: string): void {
    if (this._wasmPThreadLib === undefined) {
      this._wasmPThreadLib = wasmPThreadLib;
    }
  }

  public static setSdk(sdk: string): void {
    Porcupine._sdk = sdk;
  }

  /**
   * Creates an instance of the Porcupine wake word engine using either
   * a '.pv' file in public directory or a base64'd string.
   *
   * @param accessKey AccessKey generated by Picovoice Console.
   * @param keywords - Built-in or object containing a base64
   * representation of or path to public binary of a Porcupine keyword model and their sensitivities.
   * Can be provided as an array or a single keyword.
   * @param keywordDetectionCallback User-defined callback to run after a keyword is detected.
   * @param model object containing a base64 string
   * representation of or path to public binary of a Porcupine parameter model used to initialize Porcupine.
   * @param model.base64 The model in base64 string to initialize Leopard.
   * @param model.publicPath The model path relative to the public directory.
   * @param model.customWritePath Custom path to save the model in storage.
   * Set to a different name to use multiple models across `Porcupine` instances.
   * @param model.forceWrite Flag to overwrite the model in storage even if it exists.
   * @param model.version Leopard model version. Set to a higher number to update the model file.
   * @param options Optional configuration arguments, see PorcupineOptions for details.
   * @param options.device String representation of the device (e.g., CPU or GPU) to use. If set to `best`, the most
   * suitable device is selected automatically. If set to `gpu`, the engine uses the first available GPU device. To
   * select a specific GPU device, set this argument to `gpu:${GPU_INDEX}`, where `${GPU_INDEX}` is the index of the
   * target GPU. If set to `cpu`, the engine will run on the CPU with the default number of threads. To specify the
   * number of threads, set this argument to `cpu:${NUM_THREADS}`, where `${NUM_THREADS}` is the desired number of
   * threads.
   * @param options.processErrorCallback User-defined callback invoked if any error happens
   * while processing the audio stream. Its only input argument is the error message.
   *
   * @returns An instance of the Porcupine engine.
   */
  public static async create(
    accessKey: string,
    keywords:
      | Array<PorcupineKeyword | BuiltInKeyword>
      | PorcupineKeyword
      | BuiltInKeyword,
    keywordDetectionCallback: DetectionCallback,
    model: PorcupineModel,
    options: PorcupineOptions = {}
  ): Promise<Porcupine> {
    const [keywordPaths, keywordLabels, sensitivities] = await keywordsProcess(
      keywords
    );

    const customWritePath = model.customWritePath
      ? model.customWritePath
      : 'porcupine_model';
    const modelPath = await loadModel({ ...model, customWritePath });

    return this._init(
      accessKey,
      keywordPaths,
      keywordLabels,
      keywordDetectionCallback,
      sensitivities,
      modelPath,
      options
    );
  }

  public static async _init(
    accessKey: string,
    keywordPaths: Array<string>,
    keywordLabels: Array<string>,
    keywordDetectionCallback: DetectionCallback,
    sensitivities: Float32Array,
    modelPath: string,
    options: PorcupineOptions = {}
  ): Promise<Porcupine> {
    if (!isAccessKeyValid(accessKey)) {
      throw new PorcupineErrors.PorcupineInvalidArgumentError('Invalid AccessKey');
    }

    let { device } = options;
    const { processErrorCallback } = options;

    const isSimd = await simd();
    if (!isSimd) {
      throw new PorcupineErrors.PorcupineRuntimeError('Browser not supported.');
    }

    const isWorkerScope = typeof WorkerGlobalScope !== 'undefined' && self instanceof WorkerGlobalScope;
    if (!isWorkerScope) {
      if (device && device !== "cpu:1") {
        console.warn("Multi-threading is not supported on main thread.");
      }
      device = "cpu:1";
    }

    if (
      keywordPaths.length !== keywordLabels.length ||
      keywordPaths.length !== sensitivities.length
    ) {
      throw new PorcupineErrors.PorcupineInvalidArgumentError(`Number of keyword paths given (${keywordPaths.length}) does not match number of 
          keyword labels (${keywordLabels.length}) or sensitivities (${sensitivities.length})`);
    }

    const sabDefined = (typeof SharedArrayBuffer !== 'undefined') && (device !== "cpu:1");

    return new Promise<Porcupine>((resolve, reject) => {
      Porcupine._porcupineMutex
        .runExclusive(async () => {
          const wasmOutput = await Porcupine.initWasm(
            accessKey.trim(),
            modelPath,
            (device) ? device : "best",
            (sabDefined) ? this._wasmPThread : this._wasmSimd,
            (sabDefined) ? this._wasmPThreadLib : this._wasmSimdLib,
            (sabDefined) ? createModulePThread : createModuleSimd,
            keywordPaths,
            sensitivities
          );
          return new Porcupine(
            wasmOutput,
            keywordLabels,
            keywordDetectionCallback,
            processErrorCallback
          );
        })
        .then((result: Porcupine) => {
          resolve(result);
        })
        .catch((error: any) => {
          reject(error);
        });
    });
  }

  /**
   * Processes a frame of audio. The required sample rate can be retrieved from '.sampleRate' and the length
   * of frame (number of audio samples per frame) can be retrieved from '.frameLength' The audio needs to be
   * 16-bit linearly-encoded. Furthermore, the engine operates on single-channel audio.
   *
   * @param pcm A frame of audio with properties described above.
   */
  public async process(pcm: Int16Array): Promise<void> {
    if (!(pcm instanceof Int16Array)) {
      const error = new PorcupineErrors.PorcupineInvalidArgumentError(
        "The argument 'pcm' must be provided as an Int16Array"
      );
      if (this._processErrorCallback) {
        this._processErrorCallback(error);
      } else {
        // eslint-disable-next-line no-console
        console.error(error);
      }
    }

    this._processMutex
      .runExclusive(async () => {
        if (this._module === undefined) {
          throw new PorcupineErrors.PorcupineInvalidStateError(
            'Attempted to call Porcupine process after release.'
          );
        }

        this._module.HEAP16.set(pcm, this._inputBufferAddress / Int16Array.BYTES_PER_ELEMENT);

        const status = this._module._pv_porcupine_process(
          this._objectAddress,
          this._inputBufferAddress,
          this._keywordIndexAddress
        );

        if (status !== PvStatus.SUCCESS) {
          const messageStack = await Porcupine.getMessageStack(
            this._module._pv_get_error_stack,
            this._module._pv_free_error_stack,
            this._messageStackAddressAddressAddress,
            this._messageStackDepthAddress,
            this._module.HEAP32,
            this._module.HEAPU8
          );

          const error = pvStatusToException(status, "Processing failed", messageStack);
          if (this._processErrorCallback) {
            this._processErrorCallback(error);
          } else {
            // eslint-disable-next-line no-console
            console.error(error);
          }
        }

        const keywordIndex = this._module.HEAP32[this._keywordIndexAddress / Int32Array.BYTES_PER_ELEMENT];
        if (keywordIndex !== -1) {
          this._keywordDetectionCallback({
            label: this._keywordLabels.get(keywordIndex) ?? '',
            index: keywordIndex,
          });
        }
      })
      .catch((error: any) => {
        if (this._processErrorCallback) {
          this._processErrorCallback(error);
        } else {
          // eslint-disable-next-line no-console
          console.error(error);
        }
      });
  }

  /**
   * Releases resources acquired by WebAssembly module.
   */
  public async release(): Promise<void> {
    if (!this._module) {
      return;
    }
    this._module._pv_porcupine_delete(this._objectAddress);
    this._module._pv_free(this._messageStackAddressAddressAddress);
    this._module._pv_free(this._messageStackDepthAddress);
    this._module._pv_free(this._inputBufferAddress);
    this._module._pv_free(this._keywordIndexAddress);
    this._module = undefined;
  }

  async onmessage(e: MessageEvent): Promise<void> {
    switch (e.data.command) {
      case 'process':
        await this.process(e.data.inputFrame);
        break;
      default:
        // eslint-disable-next-line no-console
        console.warn(`Unrecognized command: ${e.data.command}`);
    }
  }

  private static async initWasm(
    accessKey: string,
    modelPath: string,
    device: string,
    wasmBase64: string,
    wasmLibBase64: string,
    createModuleFunc: any,
    keywordPaths: Array<string>,
    sensitivities: Float32Array
  ): Promise<any> {
    const blob = new Blob(
      [base64ToUint8Array(wasmLibBase64)],
      { type: 'application/javascript' }
    );
    const module: PorcupineModule = await createModuleFunc({
      mainScriptUrlOrBlob: blob,
      wasmBinary: base64ToUint8Array(wasmBase64),
    });

    const pv_porcupine_init: pv_porcupine_init_type = this.wrapAsyncFunction(
      module,
      "pv_porcupine_init",
      7);

    const objectAddressAddress = module._malloc(Int32Array.BYTES_PER_ELEMENT);
    if (objectAddressAddress === 0) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError(
        'malloc failed: Cannot allocate memory'
      );
    }

    const accessKeyEncoded = new TextEncoder().encode(accessKey);
    const accessKeyAddress = module._malloc((accessKey.length + 1) * Uint8Array.BYTES_PER_ELEMENT);
    if (accessKeyAddress === 0) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError(
        'malloc failed: Cannot allocate memory'
      );
    }
    module.HEAP8.set(accessKeyEncoded, accessKeyAddress);
    module.HEAP8[accessKeyAddress + accessKey.length] = 0;

    const modelPathEncoded = new TextEncoder().encode(modelPath);
    const modelPathAddress = module._malloc((modelPathEncoded.length + 1) * Uint8Array.BYTES_PER_ELEMENT);
    if (modelPathAddress === 0) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError(
        'malloc failed: Cannot allocate memory'
      );
    }

    module.HEAP8.set(modelPathEncoded, modelPathAddress);
    module.HEAP8[modelPathAddress + modelPathEncoded.length] = 0;

    const deviceEncoded = new TextEncoder().encode(device);
    const deviceAddress = module._malloc((device.length + 1) * Uint8Array.BYTES_PER_ELEMENT);
    if (deviceAddress === 0) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError(
        'malloc failed: Cannot allocate memory'
      );
    }
    module.HEAP8.set(deviceEncoded, deviceAddress);
    module.HEAPU8[deviceAddress + device.length] = 0;

    const keywordPathsAddressAddress = module._malloc(
      keywordPaths.length * Int32Array.BYTES_PER_ELEMENT
    );
    if (keywordPathsAddressAddress === 0) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError('malloc failed: Cannot allocate memory');
    }

    const keywordPathsAddressList: number[] = [];
    for (const keywordPath of keywordPaths) {
      const keywordPathEncoded = new TextEncoder().encode(keywordPath);
      const keywordPathAddress = module._malloc(
        (keywordPathEncoded.length + 1) * Uint8Array.BYTES_PER_ELEMENT
      );
      if (keywordPathAddress === 0) {
        throw new PorcupineErrors.PorcupineOutOfMemoryError('malloc failed: Cannot allocate memory');
      }
      module.HEAP8.set(keywordPathEncoded, keywordPathAddress);
      module.HEAP8[keywordPathAddress + keywordPathEncoded.length] = 0;
      keywordPathsAddressList.push(keywordPathAddress);
    }
    module.HEAP32.set(
      new Int32Array(keywordPathsAddressList),
      keywordPathsAddressAddress / Int32Array.BYTES_PER_ELEMENT
    );

    const sensitivityAddress = module._malloc(
      keywordPaths.length * Float32Array.BYTES_PER_ELEMENT
    );
    if (sensitivityAddress === 0) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError('malloc failed: Cannot allocate memory');
    }
    module.HEAPF32.set(
      sensitivities,
      sensitivityAddress / Float32Array.BYTES_PER_ELEMENT
    );

    const sdkEncoded = new TextEncoder().encode(this._sdk);
    const sdkAddress = module._malloc((sdkEncoded.length + 1) * Uint8Array.BYTES_PER_ELEMENT);
    if (!sdkAddress) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError(
        'malloc failed: Cannot allocate memory'
      );
    }
    module.HEAP8.set(sdkEncoded, sdkAddress);
    module.HEAP8[sdkAddress + sdkEncoded.length] = 0;
    module._pv_set_sdk(sdkAddress);

    const messageStackDepthAddress = module._malloc(Int32Array.BYTES_PER_ELEMENT);
    if (!messageStackDepthAddress) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError(
        'malloc failed: Cannot allocate memory'
      );
    }

    const messageStackAddressAddressAddress = module._malloc(Int32Array.BYTES_PER_ELEMENT);
    if (!messageStackAddressAddressAddress) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError(
        'malloc failed: Cannot allocate memory'
      );
    }

    const status: PvStatus = await pv_porcupine_init(
      accessKeyAddress,
      modelPathAddress,
      deviceAddress,
      keywordPaths.length,
      keywordPathsAddressAddress,
      sensitivityAddress,
      objectAddressAddress
    );

    module._pv_free(accessKeyAddress);
    module._pv_free(modelPathAddress);
    module._pv_free(deviceAddress);
    module._pv_free(keywordPathsAddressAddress);
    module._pv_free(sensitivityAddress);

    if (status !== PvStatus.SUCCESS) {
      const messageStack = await Porcupine.getMessageStack(
        module._pv_get_error_stack,
        module._pv_free_error_stack,
        messageStackAddressAddressAddress,
        messageStackDepthAddress,
        module.HEAP32,
        module.HEAPU8,
      );

      throw pvStatusToException(status, 'Initialization failed', messageStack);
    }
    const objectAddress = module.HEAP32[objectAddressAddress / Int32Array.BYTES_PER_ELEMENT];
    module._pv_free(objectAddressAddress);

    const frameLength = module._pv_porcupine_frame_length();
    const sampleRate = module._pv_sample_rate();
    const versionAddress = module._pv_porcupine_version();

    const version = arrayBufferToStringAtIndex(
      module.HEAPU8,
      versionAddress,
    );

    const keywordIndexAddress = module._malloc(Int32Array.BYTES_PER_ELEMENT);
    if (keywordIndexAddress === 0) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError('malloc failed: Cannot allocate memory');
    }

    const inputBufferAddress = module._malloc(frameLength * Int16Array.BYTES_PER_ELEMENT);
    if (inputBufferAddress === 0) {
      throw new PorcupineErrors.PorcupineOutOfMemoryError(
        'malloc failed: Cannot allocate memory'
      );
    }

    return {
      module: module,

      frameLength: frameLength,
      sampleRate: sampleRate,
      version: version,

      objectAddress: objectAddress,
      inputBufferAddress: inputBufferAddress,
      keywordIndexAddress: keywordIndexAddress,
      messageStackAddressAddressAddress: messageStackAddressAddressAddress,
      messageStackDepthAddress: messageStackDepthAddress,
    };
  }

  /**
   * Lists all available devices that Porcupine can use for inference.
   * Each entry in the list can be the used as the `device` argument for the `.create` method.
   *
   * @returns List of all available devices that Porcupine can use for inference.
   */
  public static async listAvailableDevices(): Promise<string[]> {
    return new Promise<string[]>((resolve, reject) => {
      Porcupine._porcupineMutex
        .runExclusive(async () => {
          const isSimd = await simd();
          if (!isSimd) {
            throw new PorcupineErrors.PorcupineRuntimeError('Unsupported Browser');
          }

          const blob = new Blob(
            [base64ToUint8Array(this._wasmSimdLib)],
            { type: 'application/javascript' }
          );
          const module: PorcupineModule = await createModuleSimd({
            mainScriptUrlOrBlob: blob,
            wasmBinary: base64ToUint8Array(this._wasmSimd),
          });

          const hardwareDevicesAddressAddress = module._malloc(Int32Array.BYTES_PER_ELEMENT);
          if (hardwareDevicesAddressAddress === 0) {
            throw new PorcupineErrors.PorcupineOutOfMemoryError(
              'malloc failed: Cannot allocate memory for hardwareDevices'
            );
          }

          const numHardwareDevicesAddress = module._malloc(Int32Array.BYTES_PER_ELEMENT);
          if (numHardwareDevicesAddress === 0) {
            throw new PorcupineErrors.PorcupineOutOfMemoryError(
              'malloc failed: Cannot allocate memory for numHardwareDevices'
            );
          }

          const status: PvStatus = module._pv_porcupine_list_hardware_devices(
            hardwareDevicesAddressAddress,
            numHardwareDevicesAddress
          );

          const messageStackDepthAddress = module._malloc(Int32Array.BYTES_PER_ELEMENT);
          if (!messageStackDepthAddress) {
            throw new PorcupineErrors.PorcupineOutOfMemoryError(
              'malloc failed: Cannot allocate memory for messageStackDepth'
            );
          }

          const messageStackAddressAddressAddress = module._malloc(Int32Array.BYTES_PER_ELEMENT);
          if (!messageStackAddressAddressAddress) {
            throw new PorcupineErrors.PorcupineOutOfMemoryError(
              'malloc failed: Cannot allocate memory messageStack'
            );
          }

          if (status !== PvStatus.SUCCESS) {
            const messageStack = await Porcupine.getMessageStack(
              module._pv_get_error_stack,
              module._pv_free_error_stack,
              messageStackAddressAddressAddress,
              messageStackDepthAddress,
              module.HEAP32,
              module.HEAPU8,
            );
            module._pv_free(messageStackAddressAddressAddress);
            module._pv_free(messageStackDepthAddress);

            throw pvStatusToException(
              status,
              'List devices failed',
              messageStack
            );
          }
          module._pv_free(messageStackAddressAddressAddress);
          module._pv_free(messageStackDepthAddress);

          const numHardwareDevices: number = module.HEAP32[numHardwareDevicesAddress / Int32Array.BYTES_PER_ELEMENT];
          module._pv_free(numHardwareDevicesAddress);

          const hardwareDevicesAddress = module.HEAP32[hardwareDevicesAddressAddress / Int32Array.BYTES_PER_ELEMENT];

          const hardwareDevices: string[] = [];
          for (let i = 0; i < numHardwareDevices; i++) {
            const deviceAddress = module.HEAP32[hardwareDevicesAddress / Int32Array.BYTES_PER_ELEMENT + i];
            hardwareDevices.push(arrayBufferToStringAtIndex(module.HEAPU8, deviceAddress));
          }
          module._pv_porcupine_free_hardware_devices(
            hardwareDevicesAddress,
            numHardwareDevices
          );
          module._pv_free(hardwareDevicesAddressAddress);

          return hardwareDevices;
        })
        .then((result: string[]) => {
          resolve(result);
        })
        .catch((error: any) => {
          reject(error);
        });
    });
  }

  private static async getMessageStack(
    pv_get_error_stack: pv_get_error_stack_type,
    pv_free_error_stack: pv_free_error_stack_type,
    messageStackAddressAddressAddress: number,
    messageStackDepthAddress: number,
    memoryBufferInt32: Int32Array,
    memoryBufferUint8: Uint8Array,
  ): Promise<string[]> {
    const status = pv_get_error_stack(messageStackAddressAddressAddress, messageStackDepthAddress);
    if (status !== PvStatus.SUCCESS) {
      throw pvStatusToException(status, 'Unable to get Porcupine error state');
    }

    const messageStackAddressAddress = memoryBufferInt32[messageStackAddressAddressAddress / Int32Array.BYTES_PER_ELEMENT];

    const messageStackDepth = memoryBufferInt32[messageStackDepthAddress / Int32Array.BYTES_PER_ELEMENT];
    const messageStack: string[] = [];
    for (let i = 0; i < messageStackDepth; i++) {
      const messageStackAddress = memoryBufferInt32[
        (messageStackAddressAddress / Int32Array.BYTES_PER_ELEMENT) + i
      ];
      const message = arrayBufferToStringAtIndex(memoryBufferUint8, messageStackAddress);
      messageStack.push(message);
    }

    pv_free_error_stack(messageStackAddressAddress);

    return messageStack;
  }

  protected static wrapAsyncFunction(module: PorcupineModule, functionName: string, numArgs: number): (...args: any[]) => any {
    // @ts-ignore
    return module.cwrap(
      functionName,
      "number",
      Array(numArgs).fill("number"),
      { async: true }
    );
  }
}
