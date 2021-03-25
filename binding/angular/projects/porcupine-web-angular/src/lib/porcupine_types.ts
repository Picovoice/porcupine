export type PorcupineWorkerResponse = {
  command: 'ppn-keyword' | 'ppn-ready';
  keywordLabel?: string;
};

export interface PorcupineWorkerFactory {
  create: (
    porcupineWorkerFactoryArgs: PorcupineWorkerFactoryArgs
  ) => Promise<PorcupineWorker>;
}

export type PorcupineServiceArgs = {
  /** Immediately start the microphone upon initialization? (defaults to true) */
  start?: boolean;
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

export type WorkerRequestProcess = {
  command: 'process';
  inputFrame: Int16Array;
};

export type WorkerRequestVoid = {
  command: 'reset' | 'pause' | 'resume' | 'release';
};

export type PorcupineWorkerRequestInit = {
  command: 'init';
  keywords: Array<PorcupineKeyword | string>;
};

export type PorcupineWorkerRequest =
  | PorcupineWorkerRequestInit
  | WorkerRequestProcess
  | WorkerRequestVoid;

export interface PorcupineWorker extends Omit<Worker, 'postMessage'> {
  postMessage(command: PorcupineWorkerRequest): void;
}
