import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

/**
 * Type alias for the Porcupine keywords.
 */
export type PorcupineKeyword = PorcupineKeywordCustom | PorcupineKeywordBuiltin;

/**
* Type alias for builtin keywords.
*/
export type PorcupineKeywordBuiltin = {
  builtin: string;
  sensitivity?: number;
};
 
/**
* Type alias for custom keywords.
*/
export type PorcupineKeywordCustom = {
  base64: string;
  custom: string;
  sensitivity?: number;
};
 
/**
* Type alias for PorcupineWorkerFactory arguments.
*/
export type PorcupineWorkerFactoryArgs = {
  accessKey: string;
  keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string;
  start?: boolean;
};
 
/**
* The language-specific worker factory, imported as { PorcupineWorkerFactory } from the 
* @picovoice/porcupine-web-xx-worker series of packages, where xx is the two-letter language code.
*/
export interface PorcupineWorkerFactory extends Object {
  create: (
    accessKey: string, 
    keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string,
    keywordDetectionCallback?: CallableFunction,
    processErrorCallback?: CallableFunction,
    start?: boolean) => Promise<Worker>,
};

/**
 * Type alias for Porcupine Vue Mixin.
 * Use with `Vue as VueConstructor extends {$porcupine: PorcupineVue}` to get types in typescript.
 */
export interface PorcupineVue {
  $_ppnWorker_: Worker | null;
  $_webVp_: WebVoiceProcessor | null;
  init: (
    porcupineFactoryArgs: PorcupineWorkerFactoryArgs,
    porcupineFactory: PorcupineWorkerFactory,
    keywordCallback: (label: string) => void,
    readyCallback: () => void,
    errorCallback: (error: Error) => void) => void;
  start: () => boolean;
  pause: () => boolean;
  delete: () => void;
}
 