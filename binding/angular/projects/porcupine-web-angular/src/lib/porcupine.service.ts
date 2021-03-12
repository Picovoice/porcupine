import { Injectable, OnDestroy } from '@angular/core';
import { Subject } from 'rxjs';

import WebVoiceProcessor from '@picovoice/web-voice-processor';
import type {
  PorcupineWorkerFactory,
  PorcupineServiceArgs,
  PorcupineWorkerResponse,
} from './porcupine_types';

@Injectable({
  providedIn: 'root',
})
export class PorcupineService implements OnDestroy {
  public webVoiceProcessor: WebVoiceProcessor;
  public isInit = false;
  public detection$: Subject<string> = new Subject<string>();
  private porcupineWorker: Worker;

  constructor() { }

  public pause(): boolean {
    if (this.webVoiceProcessor !== null) {
      this.webVoiceProcessor.pause();
      return true;
    }
    return false;
  }

  public start(): boolean {
    if (this.webVoiceProcessor !== null) {
      this.webVoiceProcessor.start();
      return true;
    }
    return false;
  }

  public resume(): boolean {
    if (this.webVoiceProcessor !== null) {
      this.webVoiceProcessor.resume();
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
    let { start } = porcupineServiceArgs;
    const { porcupineFactoryArgs } = porcupineServiceArgs;
    start = start ?? true;
    this.isInit = true;

    try {
      this.porcupineWorker = await porcupineWorkerFactory.create(
        porcupineFactoryArgs
      );
      this.porcupineWorker.onmessage = function(
        message: MessageEvent<PorcupineWorkerResponse>
      ) {
        switch (message.data.command) {
          case 'ppn-keyword': {
            this.detection$.next(message.data.keywordLabel);
          }
        }
      }.bind(this);
    } catch (error) {
      this.isInit = false;
      throw error;
    }

    try {
      this.webVoiceProcessor = await WebVoiceProcessor.init({
        engines: [this.porcupineWorker],
        start: start,
      });
    } catch (error) {
      this.porcupineWorker.postMessage({ command: 'release' });
      this.porcupineWorker = null;
      this.isInit = false;
      throw error;
    }
  }

  async ngOnDestroy() {
    this.release();
  }
}
