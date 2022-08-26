import { Component, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';

import { PorcupineService } from '@picovoice/porcupine-angular';

import { BuiltInKeyword } from '@picovoice/porcupine-web';

import porcupineParams from '../lib/porcupine_params';

@Component({
  selector: 'app-voice-widget',
  templateUrl: './voice_widget.component.html',
  styleUrls: ['./voice_widget.component.scss']
})
export class VoiceWidget implements OnDestroy {
  private wakeWordDetection: Subscription;
  private isLoadedDetection: Subscription;
  private isListeningDetection: Subscription;
  private errorDetection: Subscription;

  isLoaded = false;
  isListening = false;
  error: Error | string | null = null;

  detections: Array<string> = [];

  constructor(private porcupineService: PorcupineService) {
    // Subscribe to Porcupine Keyword detections
    // Store each detection, so we can display it in an HTML list
    this.wakeWordDetection = porcupineService.wakeWordDetection$.subscribe(
      porcupineDetection => {
        this.detections = [...this.detections, porcupineDetection.label];
      });

    // Subscribe to isListening, isLoaded, and error
    this.isLoadedDetection = porcupineService.isLoaded$.subscribe(
      isLoaded => {
        this.isLoaded = isLoaded;
      });
    this.isListeningDetection = porcupineService.isListening$.subscribe(
      isListening => {
        this.isListening = isListening;
      });
    this.errorDetection = porcupineService.error$.subscribe(
      error => {
        this.error = error;
      });
  }

  ngOnDestroy(): void {
    this.wakeWordDetection.unsubscribe();
    this.isLoadedDetection.unsubscribe();
    this.isListeningDetection.unsubscribe();
    this.errorDetection.unsubscribe();
    this.porcupineService.release();
  }

  public async stop(): Promise<void> {
    await this.porcupineService.stop();
  }

  public async start(): Promise<void> {
    await this.porcupineService.start();
  }

  public async initEngine(accessKey: string): Promise<void> {
    if (accessKey.length >= 0) {
      await this.porcupineService.release();
      try {
        await this.porcupineService.init(
          accessKey,
          [BuiltInKeyword.Alexa, BuiltInKeyword.Porcupine],
          { base64: porcupineParams }
        );
      }
      catch (error) {
        this.error = error;
      }
    }
  }

  public async release(): Promise<void> {
    await this.porcupineService.release();
  }
}
