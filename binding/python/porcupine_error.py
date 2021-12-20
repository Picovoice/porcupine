#
# Copyright 2021 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

class PorcupineError(Exception):
    pass


class PorcupineMemoryError(PorcupineError):
    pass


class PorcupineIOError(PorcupineError):
    pass


class PorcupineInvalidArgumentError(PorcupineError):
    pass


class PorcupineStopIterationError(PorcupineError):
    pass


class PorcupineKeyError(PorcupineError):
    pass


class PorcupineInvalidStateError(PorcupineError):
    pass


class PorcupineRuntimeError(PorcupineError):
    pass


class PorcupineActivationError(PorcupineError):
    pass


class PorcupineActivationLimitError(PorcupineError):
    pass


class PorcupineActivationThrottledError(PorcupineError):
    pass


class PorcupineActivationRefusedError(PorcupineError):
    pass
