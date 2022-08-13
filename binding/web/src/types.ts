/*
  Copyright 2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { BuiltInKeyword} from "./built_in_keywords";

export type PorcupineOptions = {
  processErrorCallback?: (error: string) => void
  /** @defaultValue 'porcupine_model' */
  customWritePath?: string;
  /** @defaultValue false */
  forceWrite?: boolean;
  /** @defaultValue 1 */
  version?: number;
};

export type PorcupineKeywordCustom = {
  /** Base64 representation of a trained Porcupine keyword (`.ppn` file) */
  base64?: string;
  /** The Porcupine keyword (`.ppn` file) path relative to the public directory */
  ppnPath?: string;
  /** An arbitrary label used for caching purposes */
  label: string;
  /** Value in range [0,1] that trades off miss rate for false alarm */
  sensitivity?: number;
  /** A flag that indicates whether the cached keyword should be used */
  usedCachedModel?: boolean;
};

export type PorcupineKeywordBuiltin = {
  /** Name of a builtin keyword for the specific language (e.g. "Grasshopper" for English, or "Ananas" for German) */
  builtin: BuiltInKeyword;
  /** Value in range [0,1] that trades off miss rate for false alarm */
  sensitivity?: number;
  /** A flag that indicates whether the cached keyword should be used */
  usedCachedModel?: boolean;
};

export type PorcupineKeyword = PorcupineKeywordCustom | PorcupineKeywordBuiltin;

export type PorcupineWorkerInitRequest = {
  command: 'init';
  accessKey: string;
  modelPath: string;
  keywords: Array<string>;
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
  keywordIndex: number;
};

export type PorcupineWorkerReleaseResponse = PorcupineWorkerFailureResponse | {
  command: 'ok';
};

export type PorcupineWorkerResponse =
  PorcupineWorkerInitResponse |
  PorcupineWorkerProcessResponse |
  PorcupineWorkerReleaseResponse;
