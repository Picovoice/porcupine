/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import {Mutex} from 'async-mutex';

import {
  PorcupineEngine,
  PorcupineKeyword,
  PorcupineWorkerResponseReady,
  PorcupineWorkerResponseFailed,
  PorcupineWorkerResponseKeyword,
  PorcupineWorkerResponseError,
  PorcupineWorkerRequest,
} from './porcupine_types';

// @ts-ignore
import Porcupine from './porcupine';

let paused = true;
let porcupineEngine: PorcupineEngine | null = null;
const processMutex = new Mutex();


async function init(accessKey: string, keywords: Array<PorcupineKeyword | string>, start = true): Promise<void> {
  let porcupineMessage: PorcupineWorkerResponseReady | PorcupineWorkerResponseFailed;
  try {
    porcupineEngine = await Porcupine.create(accessKey, keywords);
    porcupineMessage = {
      command: 'ppn-ready',
    };
  } catch (error) {
    porcupineMessage = {
      command: 'ppn-failed',
      message: error as string,
    };
  }
  paused = !start;
  // @ts-ignore
  postMessage(porcupineMessage, undefined);
}

async function process(inputFrame: Int16Array): Promise<void> {
  try {
    if (porcupineEngine !== null && !paused) {
      const keywordIndex = await porcupineEngine.process(inputFrame);
      if (keywordIndex !== -1) {
        const ppnKeywordMessage: PorcupineWorkerResponseKeyword = {
          command: 'ppn-keyword',
          keywordLabel: porcupineEngine.keywordLabels.get(keywordIndex),
        };
        postMessage(ppnKeywordMessage, undefined);
      }
    }
  } catch (error) {
    const errorMessage = String(error);
    const PorcupineErrorMessage: PorcupineWorkerResponseError = {
      command: 'ppn-error',
      message: errorMessage,
    };
    postMessage(PorcupineErrorMessage, undefined);
  }
}

async function release(): Promise<void> {
  if (porcupineEngine !== null) {
    porcupineEngine.release();
  }
  porcupineEngine = null;
  close();
}

async function internal_onmessage(data: PorcupineWorkerRequest): Promise<void> {
  await processMutex.runExclusive(async () => {
    switch (data.command) {
      case 'init':
        await init(data.accessKey, data.keywords,data.start);
        break;
      case 'process':
        await process(data.inputFrame);
        break;
      case 'pause':
        paused = true;
        break;
      case 'resume':
        paused = false;
        break;
      case 'release':
        release();
        break;
      default:
        // eslint-disable-next-line no-console
        console.warn('Unhandled command in porcupine_worker: ' + data.command);
    }
  });
}

onmessage = function (
  event: MessageEvent<PorcupineWorkerRequest>
): void {
  switch (event.data.command) {
    case 'file-save-succeeded':
      Porcupine.resolveFilePromise(event.data.message);
      Porcupine.clearFilePromises();
      break;
    case 'file-save-failed':
      Porcupine.rejectFilePromise(event.data.message);
      Porcupine.clearFilePromises();
      break;
    case 'file-load-succeeded':
      Porcupine.resolveFilePromise(event.data.content);
      Porcupine.clearFilePromises();
      break;
    case 'file-load-failed':
      Porcupine.rejectFilePromise(event.data.message);
      Porcupine.clearFilePromises();
      break;
    case 'file-exists-succeeded':
      Porcupine.resolveFilePromise(event.data.content);
      Porcupine.clearFilePromises();
      break;
    case 'file-exists-failed':
      Porcupine.rejectFilePromise(event.data.message);
      Porcupine.clearFilePromises();
      break;
    case 'file-delete-succeeded':
      Porcupine.resolveFilePromise(event.data.message);
      Porcupine.clearFilePromises();
      break;
    case 'file-delete-failed':
      Porcupine.rejectFilePromise(event.data.message);
      Porcupine.clearFilePromises();
      break;
    default:
      internal_onmessage(event.data);
  }
};
