import { Injectable, OnDestroy } from '@angular/core';
import { Subject } from 'rxjs';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';
import type {
  PorcupineWorkerFactory,
  PorcupineServiceArgs,
  PorcupineWorker,
} from './porcupine_types';

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

  private keywordCallback = (label: string): void => {
    this.keyword$.next(label);
  }

  private errorCallback = (error: string): void => {
    this.error$.next(error);
    this.isInit = true;
  }

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
      this.error$.next(error);
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
      this.error$.next(error);
      this.isError$.next(true);
      throw error;
    }

    this.isError$.next(false);
  }

  async ngOnDestroy() {
    this.release();
  }
}
