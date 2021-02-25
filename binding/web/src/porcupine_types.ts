/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import { BuiltInKeywordEn } from './built_in_keywords_en';

export enum WorkerCommand {
  Process = 'process',
  Init = 'init',
  Reset = 'reset',
  Pause = 'pause',
  Resume = 'resume',
  Release = 'release',
}

export type GenericWorkerCommand = {
  command:
    | WorkerCommand.Pause
    | WorkerCommand.Release
    | WorkerCommand.Reset
    | WorkerCommand.Resume;
};

export type WorkerProcessCommand = {
  command: WorkerCommand.Process;
  inputFrame: Int16Array;
};

export type PorcupineKeywordCustom = {
  base64: string;
  custom: string;
  sensitivity?: number;
};

export type PorcupineKeywordBuiltin = {
  builtin: BuiltInKeywordEn;
  sensitivity?: number;
};

export type PorcupineKeyword = PorcupineKeywordCustom | PorcupineKeywordBuiltin;


export type PorcupineWorkerInit = {
  command: WorkerCommand.Init;
  keywords: Array<PorcupineKeyword | string>;
};

export type PorcupineWorkerMessageOut = {
  command: 'ppn-keyword' | 'ppn-ready';
  keywordLabel?: string;
};

export interface PorcupineEngine {
  release(): void;
  process(frames: Int16Array): number;
  version: string;
  sampleRate: number;
  frameLength: number;
  keywordLabels: Map<number, string>;
}

