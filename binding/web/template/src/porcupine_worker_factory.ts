/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import PorcupineWorker from 'web-worker:./porcupine_worker.ts';
import {
  PorcupineKeyword,
  PorcupineWorkerRequestInit,
  PorcupineWorkerResponseReady,
  PorcupineWorkerResponseKeyword,
} from './porcupine_types';

import { BuiltInKeyword } from './lang/built_in_keywords';

export default class PorcupineWorkerFactory {
  private constructor() {}

  public static get BuiltInKeyword(): typeof BuiltInKeyword {
    return BuiltInKeyword;
  }

  /**
   * Create Porcupine web worker instances. The promise resolves when the worker is ready to process
   * voice data (perhaps from WebVoiceProcessor).
   *
   * @param {Array<PorcupineKeyword | string>} keywords - Built-in or Base64
   * representations of keywords and their sensitivities.
   *
   */
  public static async create(
    keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string
  ): Promise<Worker> {
    // n.b. The *worker* creation is itself synchronous. But, inside the worker is an async
    // method of PorcupineFactory which is initializing. This means the worker is not actually ready
    // for voice processing immediately after intantiation. When its initialization completes,
    // we receive a special PorcupineWorkerMessageOut message and resolve the worker promise.
    const porcupineWorker = new PorcupineWorker();

    const keywordArray = Array.isArray(keywords) ? keywords : [keywords];

    const ppnInitCmd: PorcupineWorkerRequestInit = {
      command: 'init',
      keywords: keywordArray,
    };
    porcupineWorker.postMessage(ppnInitCmd);

    const workerPromise = new Promise<Worker>((resolve, reject) => {
      porcupineWorker.onmessage = function (
        event: MessageEvent<
          PorcupineWorkerResponseReady | PorcupineWorkerResponseKeyword
        >
      ): void {
        if (event.data.command === 'ppn-ready') {
          resolve(porcupineWorker);
        } else if (event.data.command === 'ppn-keyword') {
          // The default keyword detection event event logs to console
          // eslint-disable-line
          console.log(event.data.keywordLabel);
        }
      };
    });

    return workerPromise;
  }
}
