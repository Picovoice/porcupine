import { Component, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';

import { PorcupineService } from '@picovoice/porcupine-angular';
import { BuiltInKeyword } from '@picovoice/porcupine-web';

// @ts-ignore
import porcupineParams from '../lib/porcupine_params';

declare var webkitSpeechRecognition: any;

@Component({
  selector: 'app-voice-box',
  templateUrl: './voice_box.component.html',
  styleUrls: ['./voice_box.component.scss']
})
export class VoiceBoxComponent implements OnDestroy {
  private keywordSubscription: Subscription;
  private isLoadedSubscription: Subscription;
  private isListeningSubscription: Subscription;
  private errorSubscription: Subscription;

  isLoaded = false;
  isListening = false;
  error: Error | string | null = null;

  listenTranscript = false;
  transcript: string = "";
  speech: any = null;

  constructor(private porcupineService: PorcupineService) {
    this.keywordSubscription = porcupineService.keywordDetection$.subscribe(
      porcupineSubscription => {
        switch (porcupineSubscription.label) {
          case "Okay Google": {
            // Supports Speech API? (i.e., is Chrome)
            if (typeof webkitSpeechRecognition === "undefined") {
              this.error =
                "This browser does not support the Web Speech API. The wake word will work, but transcription will not. Try it in Chrome to use transcription.";
              return;
            }

            // Already listening?
            if (this.speech === null) {
              this.speech = new webkitSpeechRecognition();
              this.speech.interimResults = true;
              this.speech.continuous = false;
              this.speech.onend = (event: any) => {
                this.listenTranscript= false;
              };
              this.speech.onresult = (event: any) => {
                this.transcript = "";
                for (const line of event.results) {
                  this.transcript += line[0].transcript.trim() + "\n\n";
                }
                this.speech = null;
              };
              this.speech.start();
              this.listenTranscript = true;
            }
            break;
          }
        }
      });
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

  public async initEngine(accessKey: string): Promise<void> {
    if (accessKey.length >= 0) {
      try {
        await this.porcupineService.init(
          accessKey,
          [BuiltInKeyword.OkayGoogle],
          { base64: porcupineParams }
        );
        await this.porcupineService.start();
      }
      catch (error: any) {
        this.error = error;
      }
    }
  }

  public async start(): Promise<void> {
    await this.porcupineService.start();
  }

  ngOnDestroy(): void {
    this.keywordSubscription.unsubscribe();
    this.isLoadedSubscription.unsubscribe();
    this.isListeningSubscription.unsubscribe();
    this.errorSubscription.unsubscribe();
    this.porcupineService.release();
  }
}
