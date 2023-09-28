/*
  Copyright 2022-2023 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { PorcupineKeyword } from './types';
import { BUILT_IN_KEYWORD_BYTES, BuiltInKeyword } from './built_in_keywords';

import {
  loadModel, PvError,
  PvModel,
} from '@picovoice/web-utils';
import * as PorcupineErrors from "./porcupine_errors";

const DEFAULT_SENSITIVITY = 0.5;

export enum PvStatus {
  SUCCESS = 10000,
  OUT_OF_MEMORY,
  IO_ERROR,
  INVALID_ARGUMENT,
  STOP_ITERATION,
  KEY_ERROR,
  INVALID_STATE,
  RUNTIME_ERROR,
  ACTIVATION_ERROR,
  ACTIVATION_LIMIT_REACHED,
  ACTIVATION_THROTTLED,
  ACTIVATION_REFUSED,
}

export async function keywordsProcess(keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword): Promise<[Array<string>, Array<string>, Float32Array]> {
  if (keywords === undefined || keywords === null) {
    throw new Error(
      'The keywords argument is undefined / empty',
    );
  }

  if (!Array.isArray(keywords)) {
    keywords = [keywords]; // eslint-disable-line
  } else if (keywords.length === 0) {
    throw new Error('The keywords argument array is empty');
  }
  const keywordSensitivities = [];
  const keywordLabels = [];
  const keywordPaths = [];

  for (const keyword of keywords) {
    // normalize keywords to PorcupineKeyword
    let keywordArgNormalized: PorcupineKeyword;
    if (typeof keyword === 'string') {
      keywordArgNormalized = {
        builtin: keyword as BuiltInKeyword,
        sensitivity: DEFAULT_SENSITIVITY,
      };
    } else if (typeof keyword !== 'object') {
      throw new Error(
        'Invalid keyword argument type: ' +
        keyword +
        ' : ' +
        typeof keyword,
      );
    } else {
      keywordArgNormalized = keyword;
    }

    if ('label' in keywordArgNormalized) {
      const customWritePath = (keywordArgNormalized.customWritePath) ? keywordArgNormalized.customWritePath : keywordArgNormalized.label;
      await loadModel({ ...keywordArgNormalized, customWritePath } as PvModel);
      keywordLabels.push(keywordArgNormalized.label);
      keywordPaths.push(customWritePath);
    } else if ('builtin' in keywordArgNormalized) {
      const validEnums = Object.values(BuiltInKeyword);
      const builtInName = keywordArgNormalized.builtin;
      // @ts-ignore
      const keywordEnum = BuiltInKeyword[builtInName.replace(' ', '')];
      if (!validEnums.includes(keywordEnum)) {
        throw new Error(
          `Keyword ${builtInName} does not map to list of built-in keywords (${validEnums})`,
        );
      }
      await loadModel({
        base64: BUILT_IN_KEYWORD_BYTES.get(keywordEnum),
        customWritePath: keywordArgNormalized.builtin,
        forceWrite: true
      });
      keywordLabels.push(keywordArgNormalized.builtin);
      keywordPaths.push(keywordArgNormalized.builtin);
    } else {
      throw new Error(
        'Unknown keyword argument: ' + JSON.stringify(keyword),
      );
    }

    keywordSensitivities.push(
      keywordArgNormalized.sensitivity ?? DEFAULT_SENSITIVITY,
    );
  }

  for (const sensitivity of keywordSensitivities) {
    if (typeof sensitivity !== 'number') {
      throw new Error('Sensitivity is not a number (in range [0,1]): ' + sensitivity);
    }
    if (sensitivity < 0 || sensitivity > 1) {
      throw new Error('Sensitivity is outside of range [0, 1]: ' + sensitivity);
    }
  }

  if (keywordSensitivities.length !== keywordLabels.length) {
    throw new Error(`keywordSensitivities (${keywordSensitivities.length}) and keywordLabels (${keywordLabels.length}) length differs`);
  }
  const sensitivities = new Float32Array(keywordSensitivities);

  return [keywordPaths, keywordLabels, sensitivities];
}

export function pvStatusToException(
  pvStatus: PvStatus,
  errorMessage: string,
  messageStack: string[] = [],
  pvError: PvError | null = null
): PorcupineErrors.PorcupineError {
  switch (pvStatus) {
    case PvStatus.OUT_OF_MEMORY:
      return new PorcupineErrors.PorcupineOutOfMemoryError(errorMessage, messageStack, pvError);
    case PvStatus.IO_ERROR:
      return new PorcupineErrors.PorcupineIOError(errorMessage, messageStack, pvError);
    case PvStatus.INVALID_ARGUMENT:
      return new PorcupineErrors.PorcupineInvalidArgumentError(errorMessage, messageStack, pvError);
    case PvStatus.STOP_ITERATION:
      return new PorcupineErrors.PorcupineStopIterationError(errorMessage, messageStack, pvError);
    case PvStatus.KEY_ERROR:
      return new PorcupineErrors.PorcupineKeyError(errorMessage, messageStack, pvError);
    case PvStatus.INVALID_STATE:
      return new PorcupineErrors.PorcupineInvalidStateError(errorMessage, messageStack, pvError);
    case PvStatus.RUNTIME_ERROR:
      return new PorcupineErrors.PorcupineRuntimeError(errorMessage, messageStack, pvError);
    case PvStatus.ACTIVATION_ERROR:
      return new PorcupineErrors.PorcupineActivationError(errorMessage, messageStack, pvError);
    case PvStatus.ACTIVATION_LIMIT_REACHED:
      return new PorcupineErrors.PorcupineActivationLimitReachedError(errorMessage, messageStack, pvError);
    case PvStatus.ACTIVATION_THROTTLED:
      return new PorcupineErrors.PorcupineActivationThrottledError(errorMessage, messageStack, pvError);
    case PvStatus.ACTIVATION_REFUSED:
      return new PorcupineErrors.PorcupineActivationRefusedError(errorMessage, messageStack, pvError);
    default:
      // eslint-disable-next-line no-console
      console.warn(`Unmapped error code: ${pvStatus}`);
      return new PorcupineErrors.PorcupineError(pvStatus, errorMessage);
  }
}
