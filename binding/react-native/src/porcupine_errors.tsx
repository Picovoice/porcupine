//
// Copyright 2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

class PorcupineError extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineError';
  }
}

class PorcupineMemoryError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineMemoryError';
  }
}

class PorcupineIOError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineIOError';
  }
}

class PorcupineInvalidArgumentError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineInvalidArgumentError';
  }
}

class PorcupineStopIterationError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineStopIterationError';
  }
}

class PorcupineKeyError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineKeyError';
  }
}

class PorcupineInvalidStateError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineInvalidStateError';
  }
}

class PorcupineRuntimeError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineRuntimeError';
  }
}

class PorcupineActivationError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineActivationError';
  }
}

class PorcupineActivationLimitError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineActivationLimitError';
  }
}

class PorcupineActivationThrottledError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineActivationThrottledError';
  }
}

class PorcupineActivationRefusedError extends PorcupineError {
  constructor(message: string) {
    super(message);
    this.name = 'PorcupineActivationRefusedError';
  }
}

export {
  PorcupineError,
  PorcupineMemoryError,
  PorcupineIOError,
  PorcupineInvalidArgumentError,
  PorcupineStopIterationError,
  PorcupineKeyError,
  PorcupineInvalidStateError,
  PorcupineRuntimeError,
  PorcupineActivationError,
  PorcupineActivationLimitError,
  PorcupineActivationThrottledError,
  PorcupineActivationRefusedError,
};
