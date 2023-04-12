/*
  Copyright 2022-2023 Picovoice Inc.

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
  BuiltInKeyword,
  PorcupineDetection,
  PorcupineModel,
  PorcupineOptions,
  PorcupineKeyword,
  PorcupineWorker,
} from '@picovoice/porcupine-web';

@Injectable({
  providedIn: 'root',
})
export class PorcupineService implements OnDestroy {
  public keywordDetection$: Subject<PorcupineDetection> = new Subject<PorcupineDetection>();

  public isLoaded$: Subject<boolean> = new Subject<boolean>();
  public isListening$: Subject<boolean> = new Subject<boolean>();
  public error$: Subject<Error | null> = new Subject<Error | null>();

  private porcupine: PorcupineWorker | null = null;

  constructor() {}

  public async init(
    accessKey: string,
    keywords:
      | Array<PorcupineKeyword | BuiltInKeyword>
      | PorcupineKeyword
      | BuiltInKeyword,
    model: PorcupineModel,
    options: PorcupineOptions = {}
  ): Promise<void> {
    if (options.processErrorCallback) {
      console.warn(
        '\'processErrorCallback\' is only supported in the Porcupine Web SDK. ' +
          'Use the \'error\' state to monitor for errors in the Angular SDK.'
      );
    }

    try {
      if (!this.porcupine) {
        this.porcupine = await PorcupineWorker.create(
          accessKey,
          keywords,
          this.keywordDetectionCallback,
          model,
          { processErrorCallback: this.errorCallback }
        );
        this.isLoaded$.next(true);
        this.error$.next(null);
      }
    } catch (error: any) {
      this.error$.next(error);
    }
  }

  public async start(): Promise<void> {
    if (this.porcupine === null) {
      this.error$.next(
        new Error('Porcupine has not been initialized or has been released')
      );
      return;
    }

    try {
      await WebVoiceProcessor.subscribe(this.porcupine);
      this.isListening$.next(true);
      this.error$.next(null);
    } catch (error: any) {
      this.error$.next(error);
      this.isListening$.next(false);
    }
  }

  public async stop(): Promise<void> {
    if (this.porcupine === null) {
      this.error$.next(
        new Error('Porcupine has not been initialized or has been released')
      );
      return;
    }

    try {
      await WebVoiceProcessor.unsubscribe(this.porcupine);
      this.isListening$.next(false);
      this.error$.next(null);
    } catch (error: any) {
      this.error$.next(error);
      this.isListening$.next(true);
    }
  }

  public async release(): Promise<void> {
    if (this.porcupine) {
      await this.stop();
      this.porcupine.terminate();
      this.porcupine = null;

      this.isLoaded$.next(false);
    }
  }

  async ngOnDestroy(): Promise<void> {
    await this.release();
  }

  private keywordDetectionCallback = (
    porcupineDetection: PorcupineDetection
  ): void => {
    this.keywordDetection$.next(porcupineDetection);
  };

  private errorCallback = (error: Error) => {
    this.error$.next(error);
  };
}
