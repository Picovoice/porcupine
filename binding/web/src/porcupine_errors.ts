//
// Copyright 2023 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import { PvError } from "@picovoice/web-utils";
import { PvStatus } from "./types";

class PorcupineError extends Error {
  private readonly _status: PvStatus;
  private readonly _shortMessage: string;
  private readonly _messageStack: string[];

  constructor(status: PvStatus, message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PorcupineError.errorToString(message, messageStack, pvError));
    this._status = status;
    this.name = 'PorcupineError';
    this._shortMessage = message;
    this._messageStack = messageStack;
  }

  get status(): PvStatus {
    return this._status;
  }

  get shortMessage(): string {
    return this._shortMessage;
  }

  get messageStack(): string[] {
    return this._messageStack;
  }

  private static errorToString(
    initial: string,
    messageStack: string[],
    pvError: PvError | null = null,
  ): string {
    let msg = initial;

    if (pvError) {
      const pvErrorMessage = pvError.getErrorString();
      if (pvErrorMessage.length > 0) {
        msg += `\nDetails: ${pvErrorMessage}`;
      }
    }

    if (messageStack.length > 0) {
      msg += `: ${messageStack.reduce((acc, value, index) =>
        acc + '\n  [' + index + '] ' + value, '')}`;
    }

    return msg;
  }
}

class PorcupineOutOfMemoryError extends PorcupineError {
  constructor(message: string, messageStack?: string[], pvError: PvError | null = null) {
    super(PvStatus.OUT_OF_MEMORY, message, messageStack, pvError);
    this.name = 'PorcupineOutOfMemoryError';
  }
}

class PorcupineIOError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.IO_ERROR, message, messageStack, pvError);
    this.name = 'PorcupineIOError';
  }
}

class PorcupineInvalidArgumentError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.INVALID_ARGUMENT, message, messageStack, pvError);
    this.name = 'PorcupineInvalidArgumentError';
  }
}

class PorcupineStopIterationError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.STOP_ITERATION, message, messageStack, pvError);
    this.name = 'PorcupineStopIterationError';
  }
}

class PorcupineKeyError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.KEY_ERROR, message, messageStack, pvError);
    this.name = 'PorcupineKeyError';
  }
}

class PorcupineInvalidStateError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.INVALID_STATE, message, messageStack, pvError);
    this.name = 'PorcupineInvalidStateError';
  }
}

class PorcupineRuntimeError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.RUNTIME_ERROR, message, messageStack, pvError);
    this.name = 'PorcupineRuntimeError';
  }
}

class PorcupineActivationError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.ACTIVATION_ERROR, message, messageStack, pvError);
    this.name = 'PorcupineActivationError';
  }
}

class PorcupineActivationLimitReachedError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.ACTIVATION_LIMIT_REACHED, message, messageStack, pvError);
    this.name = 'PorcupineActivationLimitReachedError';
  }
}

class PorcupineActivationThrottledError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.ACTIVATION_THROTTLED, message, messageStack, pvError);
    this.name = 'PorcupineActivationThrottledError';
  }
}

class PorcupineActivationRefusedError extends PorcupineError {
  constructor(message: string, messageStack: string[] = [], pvError: PvError | null = null) {
    super(PvStatus.ACTIVATION_REFUSED, message, messageStack, pvError);
    this.name = 'PorcupineActivationRefusedError';
  }
}

export {
  PorcupineError,
  PorcupineOutOfMemoryError,
  PorcupineIOError,
  PorcupineInvalidArgumentError,
  PorcupineStopIterationError,
  PorcupineKeyError,
  PorcupineInvalidStateError,
  PorcupineRuntimeError,
  PorcupineActivationError,
  PorcupineActivationLimitReachedError,
  PorcupineActivationThrottledError,
  PorcupineActivationRefusedError,
};



export function pvStatusToException(
  pvStatus: PvStatus,
  errorMessage: string,
  messageStack: string[] = [],
  pvError: PvError | null = null
): PorcupineError {
  switch (pvStatus) {
    case PvStatus.OUT_OF_MEMORY:
      return new PorcupineOutOfMemoryError(errorMessage, messageStack, pvError);
    case PvStatus.IO_ERROR:
      return new PorcupineIOError(errorMessage, messageStack, pvError);
    case PvStatus.INVALID_ARGUMENT:
      return new PorcupineInvalidArgumentError(errorMessage, messageStack, pvError);
    case PvStatus.STOP_ITERATION:
      return new PorcupineStopIterationError(errorMessage, messageStack, pvError);
    case PvStatus.KEY_ERROR:
      return new PorcupineKeyError(errorMessage, messageStack, pvError);
    case PvStatus.INVALID_STATE:
      return new PorcupineInvalidStateError(errorMessage, messageStack, pvError);
    case PvStatus.RUNTIME_ERROR:
      return new PorcupineRuntimeError(errorMessage, messageStack, pvError);
    case PvStatus.ACTIVATION_ERROR:
      return new PorcupineActivationError(errorMessage, messageStack, pvError);
    case PvStatus.ACTIVATION_LIMIT_REACHED:
      return new PorcupineActivationLimitReachedError(errorMessage, messageStack, pvError);
    case PvStatus.ACTIVATION_THROTTLED:
      return new PorcupineActivationThrottledError(errorMessage, messageStack, pvError);
    case PvStatus.ACTIVATION_REFUSED:
      return new PorcupineActivationRefusedError(errorMessage, messageStack, pvError);
    default:
      // eslint-disable-next-line no-console
      console.warn(`Unmapped error code: ${pvStatus}`);
      return new PorcupineError(pvStatus, errorMessage);
  }
}
