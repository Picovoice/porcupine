export type PorcupineWorkerResponseReady = {
  command: 'ppn-ready';
};

export type PorcupineWorkerResponseFailed = {
  command: 'ppn-failed';
  message: string;
};

export type PorcupineWorkerResponseError = {
  command: 'ppn-error';
  message: string;
};

export type PorcupineWorkerResponseFileOperation = {
  command: 'file-save' | 'file-load' | 'file-exists' | 'file-delete';
  path: string;
  content?: string;
};

export type PorcupineWorkerResponseKeyword = {
  command: 'ppn-keyword';
  keywordLabel: string;
};

export type PorcupineWorkerResponse =
  | PorcupineWorkerResponseReady
  | PorcupineWorkerResponseFailed
  | PorcupineWorkerResponseKeyword
  | PorcupineWorkerResponseError
  | PorcupineWorkerResponseFileOperation;

export interface PorcupineWorkerFactory {
  create(
    accessKey: string,
    keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string,
    keywordDetectionCallback?: CallableFunction,
    processErrorCallback?: CallableFunction,
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

// Worker

export type PorcupineWorkerRequestProcess = {
  command: 'process';
  /** A frame of 16-bit 16kHz PCM audio */
  inputFrame: Int16Array;
};

export type PorcupineWorkerRequestVoid = {
  command: 'reset' | 'pause' | 'resume' | 'release';
};

export type PorcupineWorkerRequestInit = {
  command: 'init';
  accessKey: string;
  keywords: Array<PorcupineKeyword | string>;
};

export type PorcupineWorkerRequestFileOperation = {
  command:
    | 'file-save-succeeded'
    | 'file-save-failed'
    | 'file-load-succeeded'
    | 'file-load-failed'
    | 'file-exists-succeeded'
    | 'file-exists-failed'
    | 'file-delete-succeeded'
    | 'file-delete-failed';
  message: string;
  content?: string;
};

export type PorcupineWorkerRequest =
  | PorcupineWorkerRequestInit
  | PorcupineWorkerRequestProcess
  | PorcupineWorkerRequestVoid
  | PorcupineWorkerRequestFileOperation;

export interface PorcupineWorker extends Omit<Worker, 'postMessage'> {
  postMessage(command: PorcupineWorkerRequest): void;
}

export type PorcupineServiceArgs = {
  /** Immediately start the microphone upon initialization? (defaults to true) */
  start?: boolean;
  /** AccessKey obtained from Picovoice Console (https://picovoice.ai/console/) */
  accessKey: string;
  /** Arguments forwarded to PorcupineWorkerFactory */
  keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string;
};
