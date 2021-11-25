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
  PorcupineMemoryException(String? message) : super(message);
}

class PorcupineIOException extends PorcupineException {
  PorcupineIOException(String? message) : super(message);
}

class PorcupineInvalidArgumentException extends PorcupineException {
  PorcupineInvalidArgumentException(String? message) : super(message);
}

class PorcupineStopIterationException extends PorcupineException {
  PorcupineStopIterationException(String? message) : super(message);
}

class PorcupineKeyException extends PorcupineException {
  PorcupineKeyException(String? message) : super(message);
}

class PorcupineInvalidStateException extends PorcupineException {
  PorcupineInvalidStateException(String? message) : super(message);
}

class PorcupineRuntimeException extends PorcupineException {
  PorcupineRuntimeException(String? message) : super(message);
}

class PorcupineActivationException extends PorcupineException {
  PorcupineActivationException(String? message) : super(message);
}

class PorcupineActivationLimitException extends PorcupineException {
  PorcupineActivationLimitException(String? message) : super(message);
}

class PorcupineActivationThrottledException extends PorcupineException {
  PorcupineActivationThrottledException(String? message) : super(message);
}

class PorcupineActivationRefusedException extends PorcupineException {
  PorcupineActivationRefusedException(String? message) : super(message);
}

porcupineStatusToException(String code, String? message) {
  switch (code) {
    case 'PorcupineException':
      return PorcupineException(message);
    case 'PorcupineMemoryException':
      return PorcupineMemoryException(message);
    case 'PorcupineIOException':
      return PorcupineIOException(message);
    case 'PorcupineInvalidArgumentException':
      return PorcupineInvalidArgumentException(message);
    case 'PorcupineStopIterationException':
      return PorcupineStopIterationException(message);
    case 'PorcupineKeyException':
      return PorcupineKeyException(message);
    case 'PorcupineInvalidStateException':
      return PorcupineInvalidStateException(message);
    case 'PorcupineRuntimeException':
      return PorcupineRuntimeException(message);
    case 'PorcupineActivationException':
      return PorcupineActivationException(message);
    case 'PorcupineActivationLimitException':
      return PorcupineActivationLimitException(message);
    case 'PorcupineActivationThrottledException':
      return PorcupineActivationThrottledException(message);
    case 'PorcupineActivationRefusedException':
      return PorcupineActivationRefusedException(message);
    default:
      return PorcupineException("unexpected code: $code, message: $message");
  }
}
