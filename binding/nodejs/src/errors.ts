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

export class PorcupineError extends Error {}

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


export function pvStatusToException(pvStatus: PvStatus, errorMessage: string): PorcupineError {
  switch (pvStatus) {
    case PvStatus.OUT_OF_MEMORY:
      throw new PorcupineOutOfMemoryError(errorMessage);
    case PvStatus.IO_ERROR:
      throw new PorcupineIoError(errorMessage);
    case PvStatus.INVALID_ARGUMENT:
      throw new PorcupineInvalidArgumentError(errorMessage);
    case PvStatus.STOP_ITERATION:
      throw new PorcupineStopIterationError(errorMessage);
    case PvStatus.KEY_ERROR:
      throw new PorcupineKeyError(errorMessage);
    case PvStatus.INVALID_STATE:
      throw new PorcupineInvalidStateError(errorMessage);
    case PvStatus.RUNTIME_ERROR:
      throw new PorcupineRuntimeError(errorMessage);
    case PvStatus.ACTIVATION_ERROR:
      throw new PorcupineActivationError(errorMessage);
    case PvStatus.ACTIVATION_LIMIT_REACHED:
      throw new PorcupineActivationLimitReached(errorMessage);
    case PvStatus.ACTIVATION_THROTTLED:
      throw new PorcupineActivationThrottled(errorMessage);
    case PvStatus.ACTIVATION_REFUSED:
      throw new PorcupineActivationRefused(errorMessage);
    default:
      // eslint-disable-next-line no-console
      console.warn(`Unmapped error code: ${pvStatus}`);
      throw new PorcupineError(errorMessage);
  }
}
