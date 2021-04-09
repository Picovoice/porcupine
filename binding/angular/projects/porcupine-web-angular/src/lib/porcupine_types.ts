export type PorcupineWorkerResponse = {
  command: 'ppn-keyword' | 'ppn-ready';
  keywordLabel?: string;
};

export interface PorcupineWorkerFactory {
  create(
    keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string,
    start?: boolean
  ): Promise<PorcupineWorker>;
}

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

export type PorcupineServiceArgs = {
  /** Immediately start the microphone upon initialization? (defaults to true) */
  start?: boolean;
  /** Arguments forwarded to PorcupineWorkerFactory */
  keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string;
};
