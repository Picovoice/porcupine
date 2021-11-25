//
// Copyright 2020-2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

class PvArgumentError extends Error {}
class PvStateError extends Error {}
class PvUnsupportedPlatformError extends Error {}

// pv_status_t error codes
class PvStatusOutOfMemoryError extends Error {}
class PvStatusIoError extends Error {}
class PvStatusInvalidArgumentError extends Error {}
class PvStatusStopIterationError extends Error {}
class PvStatusKeyError extends Error {}
class PvStatusInvalidStateError extends Error {}
class PvStatusRuntimeError extends Error {}
class PvStatusActivationError extends Error {}
class PvStatusActivationLimitReached extends Error {}
class PvStatusActivationThrottled extends Error {}
class PvStatusActivationRefused extends Error {}

const PV_STATUS_T = require("./pv_status_t");

function pvStatusToException(pvStatusInt, errorMessage) {
  switch (pvStatusInt) {
    case PV_STATUS_T.OUT_OF_MEMORY:
      throw new PvStatusOutOfMemoryError(errorMessage);
    case PV_STATUS_T.IO_ERROR:
      throw new PvStatusIoError(errorMessage);
    case PV_STATUS_T.INVALID_ARGUMENT:
      throw new PvStatusInvalidArgumentError(errorMessage);
    case PV_STATUS_T.STOP_ITERATION:
      throw new PvStatusStopIterationError(errorMessage);
    case PV_STATUS_T.KEY_ERROR:
      throw new PvStatusKeyError(errorMessage);
    case PV_STATUS_T.INVALID_STATE:
      throw new PvStatusInvalidStateError(errorMessage);
    case PV_STATUS_T.RUNTIME_ERROR:
      throw new PvStatusRuntimeError(errorMessage);
    case PV_STATUS_T.ACTIVATION_ERROR:
      throw new PvStatusActivationError(errorMessage);
    case PV_STATUS_T.ACTIVATION_LIMIT_REACHED:
      throw new PvStatusActivationLimitReached(errorMessage);
    case PV_STATUS_T.ACTIVATION_THROTTLED:
      throw new PvStatusActivationThrottled(errorMessage);
    case PV_STATUS_T.ACTIVATION_REFUSED:
      throw new PvStatusActivationRefused(errorMessage);      
    default:
      console.warn(`Unmapped error code: ${pvStatusInt}`);
      throw new Error(errorMessage);
  }
}

module.exports = {
  PvStatusActivationError,
  PvStatusActivationLimitReached,
  PvStatusActivationRefused,
  PvStatusActivationThrottled,
  PvArgumentError,
  PvStateError,
  PvStatusInvalidArgumentError,
  PvStatusInvalidStateError,
  PvStatusIoError,
  PvStatusKeyError,
  PvStatusOutOfMemoryError,
  PvStatusRuntimeError,
  PvStatusStopIterationError,
  pvStatusToException,
  PvUnsupportedPlatformError,
};
