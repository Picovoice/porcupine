//
// Copyright 2020-2022 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

import PvStatus from "./pv_status_t";

export class PorcupineError extends Error {
  private readonly _shortMessage: string;
  private readonly _messageStack: string[];

  constructor(message: string, messageStack: string[] = []) {
    super(PorcupineError.errorToString(message, messageStack));
    this._shortMessage = message;
    this._messageStack = messageStack;
  }

  get shortMessage(): string {
    return this._shortMessage;
  }

  get messageStack(): string[] {
    return this._messageStack;
  }

  private static errorToString(
    initial: string,
    messageStack: string[]
  ): string {
    let msg = initial;

    if (messageStack.length > 0) {
      msg += `: ${messageStack.reduce((acc, value, index) =>
        acc + '\n  [' + index + '] ' + value, '')}`;
    }

    return msg;
  }
}

export class PorcupineOutOfMemoryError extends PorcupineError {}
export class PorcupineIoError extends PorcupineError {}
export class PorcupineInvalidArgumentError extends PorcupineError {}
export class PorcupineStopIterationError extends PorcupineError {}
export class PorcupineKeyError extends PorcupineError {}
export class PorcupineInvalidStateError extends PorcupineError {}
export class PorcupineRuntimeError extends PorcupineError {}
export class PorcupineActivationError extends PorcupineError {}
export class PorcupineActivationLimitReached extends PorcupineError {}
export class PorcupineActivationThrottled extends PorcupineError {}
export class PorcupineActivationRefused extends PorcupineError {}


export function pvStatusToException(pvStatus: PvStatus, errorMessage: string, messageStack: string[] = [],): PorcupineError {
  switch (pvStatus) {
    case PvStatus.OUT_OF_MEMORY:
      throw new PorcupineOutOfMemoryError(errorMessage, messageStack);
    case PvStatus.IO_ERROR:
      throw new PorcupineIoError(errorMessage, messageStack);
    case PvStatus.INVALID_ARGUMENT:
      throw new PorcupineInvalidArgumentError(errorMessage, messageStack);
    case PvStatus.STOP_ITERATION:
      throw new PorcupineStopIterationError(errorMessage, messageStack);
    case PvStatus.KEY_ERROR:
      throw new PorcupineKeyError(errorMessage, messageStack);
    case PvStatus.INVALID_STATE:
      throw new PorcupineInvalidStateError(errorMessage, messageStack);
    case PvStatus.RUNTIME_ERROR:
      throw new PorcupineRuntimeError(errorMessage, messageStack);
    case PvStatus.ACTIVATION_ERROR:
      throw new PorcupineActivationError(errorMessage, messageStack);
    case PvStatus.ACTIVATION_LIMIT_REACHED:
      throw new PorcupineActivationLimitReached(errorMessage, messageStack);
    case PvStatus.ACTIVATION_THROTTLED:
      throw new PorcupineActivationThrottled(errorMessage, messageStack);
    case PvStatus.ACTIVATION_REFUSED:
      throw new PorcupineActivationRefused(errorMessage, messageStack);
    default:
      // eslint-disable-next-line no-console
      console.warn(`Unmapped error code: ${pvStatus}`);
      throw new PorcupineError(errorMessage, messageStack);
  }
}
