/*
    Copyright 2021 Picovoice Inc.

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

export interface PorcupineEngine {
  /** Release all resources acquired by Porcupine */
  release(): Promise<void>;
  /** Process a single frame of 16-bit 16kHz PCM audio */
  process(frame: Int16Array): Promise<number>;
  /** The version of the Porcupine engine */
  readonly version: string;
  /** The sampling rate of audio expected by the Porcupine engine */
  readonly sampleRate: number;
  /** The frame length of audio expected by the Porcupine engine */
  readonly frameLength: number;
  /** Maps the keyword detection index (e.g. 0, 1) returned by Porcupine to the label (e.g. "Hey Pico", "Grasshopper") */
  readonly keywordLabels: Map<number, string>;
}

// Worker

export type PorcupineWorkerRequestProcess = {
  command: 'process';
  /** A frame of 16-bit 16kHz PCM audio */
  inputFrame: Int16Array;
};

export type PorcupineWorkerRequestVoid = {
  command: 'reset' | 'pause' | 'resume' | 'release';
};

export type PorcupineWorkerRequestInit = {
  command: 'init';
  accessKey: string;
  keywords: Array<PorcupineKeyword | string>;
};

export type PorcupineWorkerRequestFileOperation = {
  command:
    | 'file-save-succeeded'
    | 'file-save-failed'
    | 'file-load-succeeded'
    | 'file-load-failed'
    | 'file-exists-succeeded'
    | 'file-exists-failed'
    | 'file-delete-succeeded'
    | 'file-delete-failed';
  message: string;
  content?: string;
};

export type PorcupineWorkerRequest =
  | PorcupineWorkerRequestInit
  | PorcupineWorkerRequestProcess
  | PorcupineWorkerRequestVoid
  | PorcupineWorkerRequestFileOperation;

export type PorcupineWorkerResponseReady = {
  command: 'ppn-ready';
};

export type PorcupineWorkerResponseFailed = {
  command: 'ppn-failed';
  message: string;
};

export type PorcupineWorkerResponseError = {
  command: 'ppn-error';
  message: string;
};

export type PorcupineWorkerResponseFileOperation = {
  command: 'file-save' | 'file-load' | 'file-exists' | 'file-delete';
  path: string;
  content?: string;
};

export type PorcupineWorkerResponseKeyword = {
  command: 'ppn-keyword';
  keywordLabel: string;
};

export type PorcupineWorkerResponse =
  | PorcupineWorkerResponseReady
  | PorcupineWorkerResponseFailed
  | PorcupineWorkerResponseKeyword
  | PorcupineWorkerResponseError
  | PorcupineWorkerResponseFileOperation;

export interface PorcupineWorker extends Omit<Worker, 'postMessage'> {
  postMessage(command: PorcupineWorkerRequest): void;
}

export interface PorcupineWorkerFactory {
  create: (
    accessKey: string,
    keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string,
    keywordDetectionCallback?: CallableFunction,
    processErrorCallback?: CallableFunction,
    start?: boolean
  ) => Promise<PorcupineWorker>;
}

// React

export type PorcupineHookArgs = {
  /** Immediately start the microphone upon initialization? */
  start: boolean;
  /** AccessKey obtained from Picovoice Console (https://picovoice.ai/console/) */
  accessKey: string;
  /** Keywords to listen for */
  keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string;
};
