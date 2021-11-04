/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

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

async function init(accessKey: string, keywords: Array<PorcupineKeyword | string>, start = true): Promise<void> {
  try {
    porcupineEngine = await Porcupine.create(accessKey, keywords);
    const porcupineReadyMessage: PorcupineWorkerResponseReady = {
      command: 'ppn-ready',
    };
    paused = !start;
    // @ts-ignore
    postMessage(porcupineReadyMessage, undefined);
  } catch (error) {
    const errorMessage = String(error);
    const porcupineFailedMessage: PorcupineWorkerResponseFailed = {
      command: 'ppn-failed',
      message: errorMessage,
    };
    postMessage(porcupineFailedMessage, undefined);
  }
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
      case 'init':
        init(event.data.accessKey, event.data.keywords,event.data.start);
        break;
      case 'process':
        process(event.data.inputFrame);
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
        console.warn('Unhandled command in porcupine_worker: ' + event.data.command);
  }
};
