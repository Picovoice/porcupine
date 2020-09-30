#
# Copyright 2018-2020 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import os
from ctypes import *
from enum import Enum


class Porcupine(object):
    """Python binding for Porcupine Wake Word Engine."""

    class PicovoiceStatuses(Enum):
        SUCCESS = 0
        OUT_OF_MEMORY = 1
        IO_ERROR = 2
        INVALID_ARGUMENT = 3
        STOP_ITERATION = 4
        KEY_ERROR = 5
        INVALID_STATE = 6

    _PICOVOICE_STATUS_TO_EXCEPTION = {
        PicovoiceStatuses.OUT_OF_MEMORY: MemoryError,
        PicovoiceStatuses.IO_ERROR: IOError,
        PicovoiceStatuses.INVALID_ARGUMENT: ValueError,
        PicovoiceStatuses.STOP_ITERATION: StopIteration,
        PicovoiceStatuses.KEY_ERROR: KeyError,
        PicovoiceStatuses.INVALID_STATE: ValueError,
    }

    class CPorcupine(Structure):
        pass

    def __init__(self, library_path, model_path, keyword_file_paths, sensitivities):
        """
        Constructor.

        :param library_path: Absolute path to Porcupine's dynamic library.
        :param model_path: Absolute path to file containing model parameters.
        :param keyword_file_paths: List of absolute paths to keyword files.
        :param sensitivities: List of sensitivity parameters.
        """

        if not os.path.exists(library_path):
            raise IOError("Couldn't find Porcupine's library at '%s'." % library_path)

        library = cdll.LoadLibrary(library_path)

        if not os.path.exists(model_path):
            raise IOError("Couldn't find model file at '%s'." % model_path)

        if len(keyword_file_paths) != len(sensitivities):
            raise ValueError("Different number of sensitivity and keyword file path parameters are provided.")

        for x in keyword_file_paths:
            if not os.path.exists(os.path.expanduser(x)):
                raise IOError("Couldn't find keyword file at '%s'." % x)

        for x in sensitivities:
            if not (0 <= x <= 1):
                raise ValueError('Sensitivity should be within [0, 1].')

        self._num_keywords = len(keyword_file_paths)

        init_func = library.pv_porcupine_init
        init_func.argtypes = [
            c_char_p,
            c_int,
            POINTER(c_char_p),
            POINTER(c_float),
            POINTER(POINTER(self.CPorcupine))]
        init_func.restype = self.PicovoiceStatuses

        self._handle = POINTER(self.CPorcupine)()

        status = init_func(
            model_path.encode('utf-8'),
            self._num_keywords,
            (c_char_p * self._num_keywords)(*[os.path.expanduser(x).encode('utf-8') for x in keyword_file_paths]),
            (c_float * self._num_keywords)(*sensitivities),
            byref(self._handle))
        if status is not self.PicovoiceStatuses.SUCCESS:
            raise self._PICOVOICE_STATUS_TO_EXCEPTION[status]()

        self._delete_func = library.pv_porcupine_delete
        self._delete_func.argtypes = [POINTER(self.CPorcupine)]
        self._delete_func.restype = None

        self.process_func = library.pv_porcupine_process
        self.process_func.argtypes = [POINTER(self.CPorcupine), POINTER(c_short), POINTER(c_int)]
        self.process_func.restype = self.PicovoiceStatuses

        version_func = library.pv_porcupine_version
        version_func.argtypes = []
        version_func.restype = c_char_p
        self._version = version_func().decode('utf-8')

        self._frame_length = library.pv_porcupine_frame_length()

        self._sample_rate = library.pv_sample_rate()

    def delete(self):
        """Releases resources acquired by Porcupine's library."""

        self._delete_func(self._handle)

    def process(self, pcm):
        """
        Processes a frame of the incoming audio stream and emits the detection result.

        :param pcm: A frame of audio samples. The number of samples per frame can be attained by calling
        '.frame_length'. The incoming audio needs to have a sample rate equal to '.sample_rate' and be 16-bit
        linearly-encoded. Porcupine operates on single-channel audio.
        :return: Returns the index of detected wake-word. Indexing is 0-based and according to ordering of input keyword
        file paths. It returns -1 when no keyword is detected.
        """

        result = c_int()
        status = self.process_func(self._handle, (c_short * len(pcm))(*pcm), byref(result))
        if status is not self.PicovoiceStatuses.SUCCESS:
            raise self._PICOVOICE_STATUS_TO_EXCEPTION[status]()

        keyword_index = result.value

        return keyword_index

    @property
    def version(self):
        """Getter for version"""

        return self._version

    @property
    def frame_length(self):
        """Getter for number of audio samples per frame."""

        return self._frame_length

    @property
    def sample_rate(self):
        """Audio sample rate accepted by Picovoice."""

        return self._sample_rate
