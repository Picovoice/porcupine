import { Component, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';

import { PorcupineService } from '@picovoice/porcupine-angular';

import { BuiltInKeyword, PorcupineKeyword } from '@picovoice/porcupine-web';

// @ts-ignore
import porcupineModel from '../lib/porcupineModel';
// @ts-ignore
import porcupineKeywords from '../lib/porcupineKeywords';

if (porcupineKeywords.length === 0 && porcupineModel.publicPath.endsWith("porcupine_params.pv")) {
  for (const k in BuiltInKeyword) {
    // @ts-ignore
    porcupineKeywords.push({builtin: BuiltInKeyword[k]});
  }
}

@Component({
  selector: 'app-voice-widget',
  templateUrl: './voice_widget.component.html',
  styleUrls: ['./voice_widget.component.scss']
})
export class VoiceWidgetComponent implements OnDestroy {
  private keywordSubscription: Subscription;
  private isLoadedSubscription: Subscription;
  private isListeningSubscription: Subscription;
  private errorSubscription: Subscription;

  isLoaded = false;
  isListening = false;
  error: Error | string | null = null;

  detections: Array<string> = [];
  keywords: Array<string> = porcupineKeywords.map((k:any) => k.label ?? k.builtin);
  constructor(private porcupineService: PorcupineService) {
    // Subscribe to Porcupine Keyword detections
    // Store each detection, so we can display it in an HTML list
    this.keywordSubscription = porcupineService.keywordDetection$.subscribe(
      porcupineSubscription => {
        this.detections = [...this.detections, porcupineSubscription.label];
      });

    // Subscribe to isListening, isLoaded, and error
    this.isLoadedSubscription = porcupineService.isLoaded$.subscribe(
      isLoaded => {
        this.isLoaded = isLoaded;
      });
    this.isListeningSubscription = porcupineService.isListening$.subscribe(
      isListening => {
        this.isListening = isListening;
      });
    this.errorSubscription = porcupineService.error$.subscribe(
      error => {
        this.error = error;
      });
  }

  ngOnDestroy(): void {
    this.keywordSubscription.unsubscribe();
    this.isLoadedSubscription.unsubscribe();
    this.isListeningSubscription.unsubscribe();
    this.errorSubscription.unsubscribe();
    this.porcupineService.release();
  }

  public async stop(): Promise<void> {
    await this.porcupineService.stop();
  }

  public async start(): Promise<void> {
    await this.porcupineService.start();
  }

  public async initEngine(accessKey: string, selectedKeyword: string): Promise<void> {

    if (this.porcupineService.isLoaded$) {
      await this.porcupineService.release();
    }

    if (accessKey.length >= 0) {
      try {
        await this.porcupineService.init(
          accessKey,
          porcupineKeywords[parseInt(selectedKeyword)],
          porcupineModel
        );
      }
      catch (error: any) {
        this.error = error;
      }
    }
  }

  public async release(): Promise<void> {
    await this.porcupineService.release();
  }
}
