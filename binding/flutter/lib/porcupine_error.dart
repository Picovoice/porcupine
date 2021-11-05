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

class PorcupineException implements Exception {
  final String? message;
  PorcupineException([this.message]);
}

class PorcupineMemoryException extends PorcupineException {
  PorcupineMemoryException(String message) : super(message);
}

class PorcupineIOException extends PorcupineException {
  PorcupineIOException(String message) : super(message);
}

class PorcupineInvalidArgumentException extends PorcupineException {
  PorcupineInvalidArgumentException(String message) : super(message);
}

class PorcupineStopIterationException extends PorcupineException {
  PorcupineStopIterationException(String message) : super(message);
}

class PorcupineKeyException extends PorcupineException {
  PorcupineKeyException(String message) : super(message);
}

class PorcupineInvalidStateException extends PorcupineException {
  PorcupineInvalidStateException(String message) : super(message);
}

class PorcupineRuntimeException extends PorcupineException {
  PorcupineRuntimeException(String message) : super(message);
}

class PorcupineActivationException extends PorcupineException {
  PorcupineActivationException(String message) : super(message);
}

class PorcupineActivationLimitException extends PorcupineException {
  PorcupineActivationLimitException(String message) : super(message);
}

class PorcupineActivationThrottledException extends PorcupineException {
  PorcupineActivationThrottledException(String message) : super(message);
}

class PorcupineActivationRefusedException extends PorcupineException {
  PorcupineActivationRefusedException(String message) : super(message);
}

enum PorcupineStatus {
  SUCCESS,
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
  ACTIVATION_REFUSED
}

porcupineStatusToException(PorcupineStatus status, String message) {
  switch (status) {
    case PorcupineStatus.OUT_OF_MEMORY:
      throw PorcupineMemoryException(message);
    case PorcupineStatus.IO_ERROR:
      throw PorcupineIOException(message);
    case PorcupineStatus.INVALID_ARGUMENT:
      throw PorcupineInvalidArgumentException(message);
    case PorcupineStatus.STOP_ITERATION:
      throw PorcupineStopIterationException(message);
    case PorcupineStatus.KEY_ERROR:
      throw PorcupineKeyException(message);
    case PorcupineStatus.INVALID_STATE:
      throw PorcupineInvalidStateException(message);
    case PorcupineStatus.RUNTIME_ERROR:
      throw PorcupineRuntimeException(message);
    case PorcupineStatus.ACTIVATION_ERROR:
      throw PorcupineActivationException(message);
    case PorcupineStatus.ACTIVATION_LIMIT_REACHED:
      throw PorcupineActivationLimitException(message);
    case PorcupineStatus.ACTIVATION_THROTTLED:
      throw PorcupineActivationThrottledException(message);
    case PorcupineStatus.ACTIVATION_REFUSED:
      throw PorcupineActivationRefusedException(message);
    default:
      throw PorcupineException("unexpected status: $status, message: $message");
  }
}
