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
 export interface PorcupineWorkerFactory extends FunctionConstructor {
   create: (
     accessKey: String, 
     keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string,
     keywordDetectionCallback?: CallableFunction,
     processErrorCallback?: CallableFunction,
     start?: boolean) => Promise<Worker>,
 };
 