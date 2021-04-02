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
  PorcupineWorkerResponseKeyword,
  PorcupineWorkerRequest,
} from './porcupine_types';

// @ts-ignore
import Porcupine from './porcupine';

let paused = true;
let porcupineEngine: PorcupineEngine = null;

async function init(keywords: Array<PorcupineKeyword | string>, start = true): Promise<void> {
  porcupineEngine = await Porcupine.create(keywords);
  paused = !start;
  const ppnReadyMessage: PorcupineWorkerResponseReady = {
    command: 'ppn-ready',
  };
  postMessage(ppnReadyMessage, undefined);
}

function process(inputFrame: Int16Array): void {
  if (porcupineEngine !== null && !paused) {
    const keywordIndex = porcupineEngine.process(inputFrame);
    if (keywordIndex !== -1) {
      const ppnKeywordMessage: PorcupineWorkerResponseKeyword = {
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
  event: MessageEvent<PorcupineWorkerRequest>
): void {
  switch (event.data.command) {
    case 'init':
      init(event.data.keywords, event.data.start);
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
      console.warn(
        'Unhandled command in porcupine_worker: ' + event.data.command
      );
  }
};
