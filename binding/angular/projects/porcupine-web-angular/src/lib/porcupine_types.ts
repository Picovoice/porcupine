export type PorcupineWorkerResponse = {
  command: 'ppn-keyword' | 'ppn-ready';
  keywordLabel?: string;
};

export interface PorcupineWorkerFactory {
  create: (
    porcupineWorkerFactoryArgs: PorcupineWorkerFactoryArgs
  ) => Promise<Worker>;
}

export type PorcupineServiceArgs = {
  /** Immediately start the microphone upon initialization? */
  start: boolean;
  /** Arguments forwarded to PorcupineWorkerFactory */
  porcupineFactoryArgs: PorcupineWorkerFactoryArgs;
};

export type PorcupineWorkerFactoryArgs =
  | Array<PorcupineKeyword | string>
  | PorcupineKeyword
  | string;

export type PorcupineKeywordCustom = {
  base64: string;
  custom: string;
  sensitivity?: number;
};

export type PorcupineKeywordBuiltin = {
  builtin: string;
  sensitivity?: number;
};

export type PorcupineKeyword = PorcupineKeywordCustom | PorcupineKeywordBuiltin;
