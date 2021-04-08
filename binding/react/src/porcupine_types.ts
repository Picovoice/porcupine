/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

// Porcupine

export type PorcupineKeywordCustom = {
  base64: string;
  custom: string;
  sensitivity?: number;
};

export type PorcupineKeywordBuiltin = {
  builtin: string;
  sensitivity?: number;
};

export type PorcupineKeyword = PorcupineKeywordCustom | PorcupineKeywordBuiltin;

export interface PorcupineEngine {
  release(): void;
  process(frames: Int16Array): number;
  version: string;
  sampleRate: number;
  frameLength: number;
  keywordLabels: Map<number, string>;
}

// Worker

export type PorcupineWorkerFactoryArgs =
  | Array<PorcupineKeyword | string>
  | PorcupineKeyword
  | string;

export interface PorcupineWorkerFactory {
  create: (
    porcupineWorkerFactoryArgs: PorcupineWorkerFactoryArgs
  ) => Promise<Worker>;
}

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

export interface PorcupineWorker extends Omit<Worker, 'postMessage'> {
  postMessage(command: PorcupineWorkerRequest): void;
}

// React

export type PorcupineHookArgs = {
  /** Immediately start the microphone upon initialization? */
  start: boolean;
  /** Keywords to listen for */
  keywords: Array<PorcupineKeyword | string>
  | PorcupineKeyword
  | string;
};
