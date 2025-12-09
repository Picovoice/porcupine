import { Porcupine } from './porcupine';
import { PorcupineWorker } from './porcupine_worker';

import {
  DetectionCallback,
  PorcupineDetection,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PorcupineWorkerFailureResponse,
  PorcupineWorkerInitRequest,
  PorcupineWorkerInitResponse,
  PorcupineWorkerProcessRequest,
  PorcupineWorkerProcessResponse,
  PorcupineWorkerReleaseRequest,
  PorcupineWorkerReleaseResponse,
  PorcupineWorkerRequest,
  PorcupineWorkerResponse,
} from './types';

import porcupineWasmSimd from './lib/pv_porcupine_simd.wasm';
import porcupineWasmSimdLib from './lib/pv_porcupine_simd.txt';
import porcupineWasmPThread from './lib/pv_porcupine_pthread.wasm';
import porcupineWasmPThreadLib from './lib/pv_porcupine_pthread.txt';

import { BuiltInKeyword } from './built_in_keywords';

import { keywordsProcess } from './utils';
import * as PorcupineErrors from './porcupine_errors';

Porcupine.setWasmSimd(porcupineWasmSimd);
Porcupine.setWasmSimdLib(porcupineWasmSimdLib);
Porcupine.setWasmPThread(porcupineWasmPThread);
Porcupine.setWasmPThreadLib(porcupineWasmPThreadLib);
PorcupineWorker.setWasmSimd(porcupineWasmSimd);
PorcupineWorker.setWasmSimdLib(porcupineWasmSimdLib);
PorcupineWorker.setWasmPThread(porcupineWasmPThread);
PorcupineWorker.setWasmPThreadLib(porcupineWasmPThreadLib);

export {
  BuiltInKeyword,
  DetectionCallback,
  keywordsProcess,
  Porcupine,
  PorcupineDetection,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PorcupineWorker,
  PorcupineWorkerFailureResponse,
  PorcupineWorkerInitRequest,
  PorcupineWorkerInitResponse,
  PorcupineWorkerProcessRequest,
  PorcupineWorkerProcessResponse,
  PorcupineWorkerReleaseRequest,
  PorcupineWorkerReleaseResponse,
  PorcupineWorkerRequest,
  PorcupineWorkerResponse,
  PorcupineErrors,
};
