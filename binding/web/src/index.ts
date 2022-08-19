import { Porcupine } from './porcupine';
import { PorcupineWorker } from './porcupine_worker';

import {
  PorcupineDetection,
  PorcupineKeyword,
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

import porcupineWasm from '../lib/pv_porcupine.wasm';
import porcupineWasmSimd from '../lib/pv_porcupine_simd.wasm';

import { BuiltInKeyword } from './built_in_keywords';

Porcupine.setWasm(porcupineWasm);
Porcupine.setWasmSimd(porcupineWasmSimd);
PorcupineWorker.setWasm(porcupineWasm);
PorcupineWorker.setWasmSimd(porcupineWasmSimd);

export {
  BuiltInKeyword,
  Porcupine,
  PorcupineDetection,
  PorcupineOptions,
  PorcupineKeyword,
  PorcupineWorker,
  PorcupineWorkerInitRequest,
  PorcupineWorkerProcessRequest,
  PorcupineWorkerReleaseRequest,
  PorcupineWorkerRequest,
  PorcupineWorkerInitResponse,
  PorcupineWorkerProcessResponse,
  PorcupineWorkerReleaseResponse,
  PorcupineWorkerFailureResponse,
  PorcupineWorkerResponse,
};
