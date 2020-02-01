#
# Copyright 2018 Picovoice Inc.
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
    """Python binding for Picovoice's wake word detection (Porcupine) engine."""

    class PicovoiceStatuses(Enum):
        """Status codes corresponding to 'pv_status_t' defined in 'include/picovoice.h'"""

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

    def __init__(
            self,
            library_path,
            model_file_path,
            keyword_file_path=None,
            sensitivity=None,
            keyword_file_paths=None,
            sensitivities=None):
        """
        Constructor.

        :param library_path: Absolute path to Porcupine's dynamic library.
        :param model_file_path: Absolute path to file containing model parameters.
        :param keyword_file_path: Absolute path to keyword file containing hyper-parameters. If not present then
        'keyword_file_paths' will be used.
        :param sensitivity: Sensitivity parameter. A higher sensitivity value lowers miss rate at the cost of increased
        false alarm rate. For more information regarding this parameter refer to 'include/pv_porcupine.h'. If not
        present then 'sensitivities' is used.
        :param keyword_file_paths: List of absolute paths to keyword files. Intended to be used for multiple keyword
        scenario. This parameter is used only when 'keyword_file_path' is not set.
        :param sensitivities: List of sensitivity parameters. Intended to be used for multiple keyword scenario. This
        parameter is used only when 'sensitivity' is not set.
        """

        if not os.path.exists(library_path):
            raise IOError("could'nt find Porcupine's library at '%s'" % library_path)

        library = cdll.LoadLibrary(library_path)

        if not os.path.exists(model_file_path):
            raise IOError("could'nt find model file at '%s'" % model_file_path)

        if sensitivity is not None and keyword_file_path is not None:
            if not os.path.exists(keyword_file_path):
                raise IOError("could'nt' find keyword file at '%s'" % keyword_file_path)
            keyword_file_paths = [keyword_file_path]

            if not (0 <= sensitivity <= 1):
                raise ValueError('sensitivity should be within [0, 1]')
            sensitivities = [sensitivity]
        elif sensitivities is not None and keyword_file_paths is not None:
            if len(keyword_file_paths) != len(sensitivities):
                raise ValueError("different number of sensitivity and keyword file path parameters are provided.")

            for x in keyword_file_paths:
                if not os.path.exists(os.path.expanduser(x)):
                    raise IOError("could not find keyword file at '%s'" % x)

            for x in sensitivities:
                if not (0 <= x <= 1):
                    raise ValueError('sensitivity should be within [0, 1]')
        else:
            raise ValueError("sensitivity and/or keyword file path is missing")

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
            model_file_path.encode('utf-8'),
            self._num_keywords,
            (c_char_p * self._num_keywords)(*[os.path.expanduser(x).encode('utf-8') for x in keyword_file_paths]),
            (c_float * self._num_keywords)(*sensitivities),
            byref(self._handle))
        if status is not self.PicovoiceStatuses.SUCCESS:
            raise self._PICOVOICE_STATUS_TO_EXCEPTION[status]('initialization failed')

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
        :return: For a single wake-word use cse True if wake word is detected. For multiple wake-word use case it
        returns the index of detected wake-word. Indexing is 0-based and according to ordering of input keyword file
        paths. It returns -1 when no keyword is detected.
        """

        result = c_int()
        status = self.process_func(self._handle, (c_short * len(pcm))(*pcm), byref(result))
        if status is not self.PicovoiceStatuses.SUCCESS:
            raise self._PICOVOICE_STATUS_TO_EXCEPTION[status]()

        keyword_index = result.value

        if self._num_keywords == 1:
            return keyword_index == 0
        else:
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
