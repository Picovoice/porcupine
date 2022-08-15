/*
  Copyright 2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

/// <reference no-default-lib="false"/>
/// <reference lib="webworker" />

import { Porcupine } from './porcupine';
import { PorcupineWorkerRequest } from './types';

/**
 * Porcupine worker handler.
 */
let porcupine: Porcupine | null = null;
self.onmessage = async function(
  event: MessageEvent<PorcupineWorkerRequest>,
): Promise<void> {
  switch (event.data.command) {
    case 'init':
      if (porcupine !== null) {
        self.postMessage({
          command: 'error',
          message: 'Porcupine already initialized',
        });
        return;
      }
      try {
        Porcupine.setWasm(event.data.wasm);
        Porcupine.setWasmSimd(event.data.wasmSimd);
        porcupine = await Porcupine.create(
          event.data.accessKey,
          event.data.keywords,
          event.data.sensitivities,
          event.data.modelPath);
        self.postMessage({
          command: 'ok',
          version: porcupine.version,
          frameLength: porcupine.frameLength,
          sampleRate: porcupine.sampleRate,
        });
      } catch (e: any) {
        self.postMessage({
          command: 'error',
          message: e.message,
        });
      }
      break;
    case 'process':
      if (porcupine === null) {
        self.postMessage({
          command: 'error',
          message: 'Porcupine not initialized',
        });
        return;
      }
      try {
        const keywordIndex = await porcupine.process(event.data.inputFrame);
        if (keywordIndex !== -1) {
          self.postMessage({
            command: 'ok',
            keywordIndex: keywordIndex,
          });
        }
      } catch (e: any) {
        self.postMessage({
          command: 'error',
          message: e.message,
        });
      }
      break;
    case 'release':
      if (porcupine !== null) {
        await porcupine.release();
        porcupine = null;
        close();
      }
      self.postMessage({
        command: 'ok',
      });
      break;
    default:
      self.postMessage({
        command: 'failed',
        // @ts-ignore
        message: `Unrecognized command: ${event.data.command}`,
      });
  }
};
