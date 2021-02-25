/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import {
  GenericWorkerCommand,
  PorcupineEngine,
  PorcupineKeyword,
  PorcupineWorkerInit,
  PorcupineWorkerMessageOut,
  WorkerCommand,
  WorkerProcessCommand,
} from './porcupine_types';

// @ts-ignore
import PorcupineEn from './porcupine_en';

let paused = true;
let porcupineEngine: PorcupineEngine = null;

async function init(keywords: Array<PorcupineKeyword | string>): Promise<void> {
  porcupineEngine = await PorcupineEn.create(keywords);
  paused = false; // TODO option to start paused?
  const ppnReadyMessage: PorcupineWorkerMessageOut = { command: 'ppn-ready' };
  postMessage(ppnReadyMessage, undefined);
}

function process(inputFrame: Int16Array): void {
  if (porcupineEngine !== null && !paused) {
    const keywordIndex = porcupineEngine.process(inputFrame);
    if (keywordIndex !== -1) {
      const ppnKeywordMessage: PorcupineWorkerMessageOut = {
        command: 'ppn-keyword',
        keywordLabel: porcupineEngine.keywordLabels.get(keywordIndex),
      };
      postMessage(ppnKeywordMessage, undefined);
    }
  }
}

function release(): void {
  if (porcupineEngine !== null) {
    porcupineEngine.release();
  }

  porcupineEngine = null;
  close();
}

onmessage = function (
  event: MessageEvent<
    WorkerProcessCommand | GenericWorkerCommand | PorcupineWorkerInit
  >
): void {
  switch (event.data.command) {
    case WorkerCommand.Init:
      init(event.data.keywords);
      break;
    case WorkerCommand.Process:
      process(event.data.inputFrame);
      break;
    case WorkerCommand.Pause:
      paused = true;
      break;
    case WorkerCommand.Resume:
      paused = false;
      break;
    case WorkerCommand.Release:
      release();
      break;
    default:
      console.warn("Unhandled command in porcupine_worker_en: " + event.data.command)
  }
};
