/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

export type PorcupineKeywordCustom = {
  /** Base64 representation of a trained Porcupine keyword (`.ppn` file) */
  base64: string;
  /** An arbitrary label that you want Picovoice to report when the detection occurs */
  custom: string;
  /** Value in range [0,1] that trades off miss rate for false alarm */
  sensitivity?: number;
};

export type PorcupineKeywordBuiltin = {
  /** Name of a builtin keyword for the specific language (e.g. "Grasshopper" for English, or "Ananas" for German) */
  builtin: string;
  /** Value in range [0,1] that trades off miss rate for false alarm */
  sensitivity?: number;
};

export type PorcupineKeyword = PorcupineKeywordCustom | PorcupineKeywordBuiltin;

export type WorkerRequestProcess = {
  command: 'process';
  inputFrame: Int16Array;
};

export type WorkerRequestVoid = {
  command: 'reset' | 'pause' | 'resume' | 'release';
};

export type PorcupineWorkerRequestInit = {
  command: 'init';
  keywords: Array<PorcupineKeyword | string>;
  start?: boolean;
};

export type PorcupineWorkerRequest = PorcupineWorkerRequestInit | WorkerRequestProcess | WorkerRequestVoid

export type PorcupineWorkerResponseReady = {
  command: 'ppn-ready';
};

export type PorcupineWorkerResponseKeyword = {
  command: 'ppn-keyword';
  keywordLabel: string;
};

export type PorcupineWorkerResponse = PorcupineWorkerResponseReady | PorcupineWorkerResponseKeyword

export interface PorcupineEngine {
  /** Release all resources acquired by Porcupine */
  release(): void;
  /** Process a single frame of 16-bit 16kHz PCM audio */
  process(frame: Int16Array): number;
  /** The version of the Porcupine engine */
  readonly version: string;
  /** The sampling rate of audio expected by the Porcupine engine */
  readonly sampleRate: number;
  /** The frame length of audio expected by the Porcupine engine */
  readonly frameLength: number;
  /** Maps the keyword detection index (e.g. 0, 1) returned by Porcupine to the label (e.g. "Hey Pico", "Grasshopper") */
  readonly keywordLabels: Map<number, string>;
}
