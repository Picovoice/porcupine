import { Component } from "@angular/core"
import { Subscription } from "rxjs"

import { PorcupineService, PorcupineServiceArgs } from "@picovoice/porcupine-web-angular"

import { DEEP_SKY_BLUE_PPN_64 } from "./porcupine_keywords"
import { PorcupineKeyword } from "@picovoice/porcupine-web-core"
import {BuiltInKeyword} from "@picovoice/porcupine-web-en-worker";

@Component({
  selector: 'voice-widget',
  templateUrl: './voice_widget.component.html',
  styleUrls: ['./voice_widget.component.scss']
})
export class VoiceWidget {
  private keywordDetection: Subscription
  private isLoadedDetection: Subscription
  private isListeningDetection: Subscription
  private errorDetection: Subscription

  title: "voice-widget"
  isLoaded: boolean = false
  isListening: boolean | null = null
  error: Error | string | null = null

  detections: Array<string> = []

  constructor(private porcupineService: PorcupineService) {
    // Subscribe to Porcupine Keyword detections
    // Store each detection so we can display it in an HTML list
    this.keywordDetection = porcupineService.keyword$.subscribe(
      porcupineDetection => {
        this.detections = [...this.detections, porcupineDetection.label]
      })

    // Subscribe to isListening, isLoaded, and error
    this.isLoadedDetection = porcupineService.isLoaded$.subscribe(
      isLoaded => {
        this.isLoadedDetection = isLoaded
      })
    this.isListeningDetection = porcupineService.isListening$.subscribe(
      isListening => {
        this.isListening = isListening
      })
    this.errorDetection = porcupineService.error$.subscribe(
      error => {
        this.error = error
      })
  }

  async ngOnInit() {
  }

  ngOnDestroy() {
    this.keywordDetection.unsubscribe()
    this.isLoadedDetection.unsubscribe()
    this.isListeningDetection.unsubscribe()
    this.errorDetection.unsubscribe()
    this.porcupineService.release()
  }

  public stop() {
    this.porcupineService.stop();
  }

  public start() {
    this.porcupineService.start();
  }

  public async initEngine(accessKey: string) {
    if (accessKey.length >= 0) {
      this.porcupineService.release();
      try {
        await this.porcupineService.init(
          accessKey,
          [BuiltInKeyword.Porcupine],
          { base64: porcupineParams }
        )
      }
      catch (error) {
        this.error = error
      }
    }
  }
}
