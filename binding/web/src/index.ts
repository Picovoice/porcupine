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

import porcupineWasm from '../lib/pv_porcupine.wasm';
import porcupineWasmSimd from '../lib/pv_porcupine_simd.wasm';

import { BuiltInKeyword } from './built_in_keywords';

import { keywordsProcess } from './utils';

Porcupine.setWasm(porcupineWasm);
Porcupine.setWasmSimd(porcupineWasmSimd);
PorcupineWorker.setWasm(porcupineWasm);
PorcupineWorker.setWasmSimd(porcupineWasmSimd);

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
};
