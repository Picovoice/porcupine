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

import PvStatusT from "./pv_status_t";

export class PvArgumentError extends Error {}
export class PvStateError extends Error {}
export class PvUnsupportedPlatformError extends Error {}

// pv_status_t error codes
export class PvStatusOutOfMemoryError extends Error {}
export class PvStatusIoError extends Error {}
export class PvStatusInvalidArgumentError extends Error {}
export class PvStatusStopIterationError extends Error {}
export class PvStatusKeyError extends Error {}
export class PvStatusInvalidStateError extends Error {}
export class PvStatusRuntimeError extends Error {}
export class PvStatusActivationError extends Error {}
export class PvStatusActivationLimitReached extends Error {}
export class PvStatusActivationThrottled extends Error {}
export class PvStatusActivationRefused extends Error {}


export function pvStatusToException(pvStatusInt: PvStatusT, errorMessage: string) {
  switch (pvStatusInt) {
    case PvStatusT.OUT_OF_MEMORY:
      throw new PvStatusOutOfMemoryError(errorMessage);
    case PvStatusT.IO_ERROR:
      throw new PvStatusIoError(errorMessage);
    case PvStatusT.INVALID_ARGUMENT:
      throw new PvStatusInvalidArgumentError(errorMessage);
    case PvStatusT.STOP_ITERATION:
      throw new PvStatusStopIterationError(errorMessage);
    case PvStatusT.KEY_ERROR:
      throw new PvStatusKeyError(errorMessage);
    case PvStatusT.INVALID_STATE:
      throw new PvStatusInvalidStateError(errorMessage);
    case PvStatusT.RUNTIME_ERROR:
      throw new PvStatusRuntimeError(errorMessage);
    case PvStatusT.ACTIVATION_ERROR:
      throw new PvStatusActivationError(errorMessage);
    case PvStatusT.ACTIVATION_LIMIT_REACHED:
      throw new PvStatusActivationLimitReached(errorMessage);
    case PvStatusT.ACTIVATION_THROTTLED:
      throw new PvStatusActivationThrottled(errorMessage);
    case PvStatusT.ACTIVATION_REFUSED:
      throw new PvStatusActivationRefused(errorMessage);      
    default:
      console.warn(`Unmapped error code: ${pvStatusInt}`);
      throw new Error(errorMessage);
  }
}
