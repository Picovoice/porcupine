import { Component } from "@angular/core"
import { Subscription } from "rxjs"

import { PorcupineService, PorcupineServiceArgs } from "@picovoice/porcupine-web-angular"

import { DEEP_SKY_BLUE_PPN_64 } from "./porcupine_keywords"
import { PorcupineKeyword } from "@picovoice/porcupine-web-core"

@Component({
  selector: 'voice-widget',
  templateUrl: './voice_widget.component.html',
  styleUrls: ['./voice_widget.component.scss']
})
export class VoiceWidget {
  private keywordDetection: Subscription
  private listeningDetection: Subscription
  private errorDetection: Subscription
  private isErrorDetection: Subscription

  title: "voice-widget"
  isChunkLoaded: boolean = false
  isLoaded: boolean = false
  isError: boolean = false
  error: Error | string | null = null
  isListening: boolean | null = null
  errorMessage: string
  detections: Array<string> = []
  keywords: PorcupineKeyword = { custom: "Deep Sky Blue", base64: DEEP_SKY_BLUE_PPN_64, sensitivity: 0.75 }

  constructor(private porcupineService: PorcupineService) {
    // Subscribe to Porcupine Keyword detections
    // Store each detection so we can display it in an HTML list
    this.keywordDetection = porcupineService.keyword$.subscribe(
      keywordLabel => {
        this.detections = [...this.detections, keywordLabel]
      })

    // Subscribe to listening, isError, and error message
    this.listeningDetection = porcupineService.listening$.subscribe(
      listening => {
        this.isListening = listening
      })
    this.errorDetection = porcupineService.error$.subscribe(
      error => {
        this.error = error
      })
    this.isErrorDetection = porcupineService.isError$.subscribe(
      isError => {
        this.isError = isError
      })
  }

  async ngOnInit() {
  }

  ngOnDestroy() {
    this.keywordDetection.unsubscribe()
    this.listeningDetection.unsubscribe()
    this.errorDetection.unsubscribe()
    this.isErrorDetection.unsubscribe()
    this.porcupineService.release()
  }

  public pause() {
    this.porcupineService.pause();
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
      const porcupineServiceArgs: PorcupineServiceArgs = { accessKey: accessKey, keywords: this.keywords};
      // Load Porcupine worker chunk with specific language model (large ~1-2MB chunk; needs to be dynamically imported)
      const porcupineFactoryEn = (await import('@picovoice/porcupine-web-en-worker')).PorcupineWorkerFactory
      this.isChunkLoaded = true;
      console.info("Porcupine EN is loaded.")
      // Initialize Porcupine Service
      try {
        await this.porcupineService.init(porcupineFactoryEn,
          porcupineServiceArgs
        )
        console.info("Porcupine is ready!")
        this.isLoaded = true;
      }
      catch (error) {
        console.error(error)
        this.isError = true;
        this.errorMessage = error.toString();
      }
    }
  }
}