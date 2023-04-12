/*
  Copyright 2022-2023 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { BuiltInKeyword } from './built_in_keywords';
import { PvModel } from "@picovoice/web-utils";

export type PorcupineOptions = {
  /** @defaultValue '(error) => {}' */
  processErrorCallback?: (error: Error) => void
};

export type PorcupineKeywordCustom = PvModel & {
  /** An arbitrary label that you want Porcupine to report when the detection occurs */
  label: string;
  /** Value in range [0,1] that trades off miss rate for false alarm */
  /** @defaultValue 0.5 */
  sensitivity?: number;
};

export type PorcupineKeywordBuiltin = {
  /** Name of a builtin keyword */
  builtin: BuiltInKeyword;
  /** Value in range [0,1] that trades off miss rate for false alarm */
  /** @defaultValue 0.5 */
  sensitivity?: number;
};

export type PorcupineKeyword = PorcupineKeywordCustom | PorcupineKeywordBuiltin;

export type PorcupineModel = PvModel;

export type PorcupineDetection = {
  /** The index of the detected keyword */
  index: number;
  /** The label of the detected keyword */
  label: string;
}

export type DetectionCallback = (detection: PorcupineDetection) => void;

export type PorcupineWorkerInitRequest = {
  command: 'init';
  accessKey: string;
  modelPath: string;
  keywordLabels: Array<string>;
  keywordPaths: Array<string>;
  sensitivities: Float32Array;
  wasm: string;
  wasmSimd: string;
  options: PorcupineOptions;
};

export type PorcupineWorkerProcessRequest = {
  command: 'process';
  inputFrame: Int16Array;
};

export type PorcupineWorkerReleaseRequest = {
  command: 'release';
};

export type PorcupineWorkerRequest =
  PorcupineWorkerInitRequest |
  PorcupineWorkerProcessRequest |
  PorcupineWorkerReleaseRequest;

export type PorcupineWorkerFailureResponse = {
  command: 'failed' | 'error';
  message: string;
};

export type PorcupineWorkerInitResponse = PorcupineWorkerFailureResponse | {
  command: 'ok';
  frameLength: number;
  sampleRate: number;
  version: string;
};

export type PorcupineWorkerProcessResponse = PorcupineWorkerFailureResponse | {
  command: 'ok';
  porcupineDetection: PorcupineDetection;
};

export type PorcupineWorkerReleaseResponse = PorcupineWorkerFailureResponse | {
  command: 'ok';
};

export type PorcupineWorkerResponse =
  PorcupineWorkerInitResponse |
  PorcupineWorkerProcessResponse |
  PorcupineWorkerReleaseResponse;
