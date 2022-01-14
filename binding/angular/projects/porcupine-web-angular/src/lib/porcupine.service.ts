/*
  Copyright 2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { Injectable, OnDestroy } from '@angular/core';
import { Subject } from 'rxjs';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';
import {
  PorcupineKeyword,
  PorcupineWorkerFactory,
  PorcupineWorker,
} from '@picovoice/porcupine-web-core';

export type PorcupineServiceArgs = {
  /** Immediately start the microphone upon initialization? (defaults to true) */
  start?: boolean;
  /** AccessKey obtained from Picovoice Console (https://picovoice.ai/console/) */
  accessKey: string;
  /** Arguments forwarded to PorcupineWorkerFactory */
  keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string;
};

@Injectable({
  providedIn: 'root',
})
export class PorcupineService implements OnDestroy {
  public webVoiceProcessor: WebVoiceProcessor | null = null;
  public keyword$: Subject<string> = new Subject<string>();
  public listening$: Subject<boolean> = new Subject<boolean>();
  public isError$: Subject<boolean> = new Subject<boolean>();
  public error$: Subject<Error | string> = new Subject<Error | string>();
  private porcupineWorker: PorcupineWorker | null = null;
  private isInit = false;

  constructor() {}

  public pause(): boolean {
    if (this.webVoiceProcessor !== null) {
      this.webVoiceProcessor.pause();
      this.listening$.next(false);
      return true;
    }
    return false;
  }

  public start(): boolean {
    if (this.webVoiceProcessor !== null) {
      this.webVoiceProcessor.start();
      this.listening$.next(true);
      return true;
    }
    return false;
  }

  public async release(): Promise<void> {
    if (this.porcupineWorker !== null) {
      this.porcupineWorker.postMessage({ command: 'release' });
    }
    if (this.webVoiceProcessor !== null) {
      await this.webVoiceProcessor.release();
    }
    this.isInit = false;
  }

  public async init(
    porcupineWorkerFactory: PorcupineWorkerFactory,
    porcupineServiceArgs: PorcupineServiceArgs
  ): Promise<void> {
    if (this.isInit) {
      throw new Error('Porcupine is already initialized');
    }
    const { accessKey, keywords, start = true } = porcupineServiceArgs;
    this.isInit = true;

    try {
      this.porcupineWorker = await porcupineWorkerFactory.create(
        accessKey,
        keywords,
        this.keywordCallback,
        this.errorCallback
      );
    } catch (error) {
      this.isInit = false;
      this.error$.next(error as Error);
      this.isError$.next(true);
      throw error;
    }

    try {
      this.webVoiceProcessor = await WebVoiceProcessor.init({
        engines: [this.porcupineWorker],
        start,
      });
      this.listening$.next(start);
    } catch (error) {
      this.porcupineWorker.postMessage({ command: 'release' });
      this.porcupineWorker = null;
      this.isInit = false;
      this.error$.next(error as Error);
      this.isError$.next(true);
      throw error;
    }

    this.isError$.next(false);
  }

  async ngOnDestroy() {
    this.release();
  }

  private keywordCallback = (label: string): void => {
    this.keyword$.next(label);
  };

  private errorCallback = (error: string | Error): void => {
    this.error$.next(error);
    this.isError$.next(true);
  };
}
