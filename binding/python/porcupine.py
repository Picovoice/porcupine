#
# Copyright 2018 Picovoice Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import os
from ctypes import *
from enum import Enum


class Porcupine(object):
    """Python binding for Picovoice's wake word detection (aka Porcupine) library."""

    class PicovoiceStatuses(Enum):
        """Status codes corresponding to 'pv_status_t' defined in 'include/picovoice.h'"""

        SUCCESS = 0
        OUT_OF_MEMORY = 1
        IO_ERROR = 2
        INVALID_ARGUMENT = 3

    _PICOVOICE_STATUS_TO_EXCEPTION = {
        PicovoiceStatuses.OUT_OF_MEMORY: MemoryError,
        PicovoiceStatuses.IO_ERROR: IOError,
        PicovoiceStatuses.INVALID_ARGUMENT: ValueError
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
        Loads Porcupine's shared library and creates an instance of wake word detection object.

        :param library_path: Absolute path to Porcupine's shared library.
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
            raise IOError("Could not find Porcupine's library at '%s'" % library_path)

        library = cdll.LoadLibrary(library_path)

        if not os.path.exists(model_file_path):
            raise IOError("Could not find model file at '%s'" % model_file_path)

        if sensitivity is not None and keyword_file_path is not None:
            if not os.path.exists(keyword_file_path):
                raise IOError("Could not find keyword file at '%s'" % keyword_file_path)
            keyword_file_paths = [keyword_file_path]

            if not (0 <= sensitivity <= 1):
                raise ValueError('Sensitivity should be within [0, 1]')
            sensitivities = [sensitivity]
        elif sensitivities is not None and keyword_file_paths is not None:
            if len(keyword_file_paths) != len(sensitivities):
                raise ValueError("Different number of sensitivity and keyword file path parameters are provided.")

            for x in keyword_file_paths:
                if not os.path.exists(os.path.expanduser(x)):
                    raise IOError("Could not find keyword file at '%s'" % x)

            for x in sensitivities:
                if not (0 <= x <= 1):
                    raise ValueError('Sensitivity should be within [0, 1]')
        else:
            raise ValueError("Sensitivity and/or keyword file path is missing")

        self._num_keywords = len(keyword_file_paths)

        init_func = library.pv_porcupine_multiple_keywords_init
        init_func.argtypes = [
            c_char_p,
            c_int,
            POINTER(c_char_p),
            POINTER(c_float),
            POINTER(POINTER(self.CPorcupine))]
        init_func.restype = self.PicovoiceStatuses

        self._handle = POINTER(self.CPorcupine)()

        status = init_func(
            model_file_path.encode(),
            self._num_keywords,
            (c_char_p * self._num_keywords)(*[os.path.expanduser(x).encode() for x in keyword_file_paths]),
            (c_float * self._num_keywords)(*sensitivities),
            byref(self._handle))
        if status is not self.PicovoiceStatuses.SUCCESS:
            raise self._PICOVOICE_STATUS_TO_EXCEPTION[status]('Initialization failed')

        self.process_func = library.pv_porcupine_multiple_keywords_process
        self.process_func.argtypes = [POINTER(self.CPorcupine), POINTER(c_short), POINTER(c_int)]
        self.process_func.restype = self.PicovoiceStatuses

        self._delete_func = library.pv_porcupine_delete
        self._delete_func.argtypes = [POINTER(self.CPorcupine)]
        self._delete_func.restype = None

        self._sample_rate = library.pv_sample_rate()
        self._frame_length = library.pv_porcupine_frame_length()

    @property
    def sample_rate(self):
        """Audio sample rate accepted by Porcupine library."""

        return self._sample_rate

    @property
    def frame_length(self):
        """Number of audio samples per frame expected by C library."""

        return self._frame_length

    def process(self, pcm):
        """
        Monitors incoming audio stream for given wake word(s).

        :param pcm: An array (or array-like) of consecutive audio samples. For more information regarding required audio
        properties (i.e. sample rate, number of channels encoding, and number of samples per frame) please refer to
        'include/pv_porcupine.h'.
        :return: For a single wake-word use cse True if wake word is detected. For multiple wake-word use case it
        returns the index of detected wake-word. Indexing is 0-based and according to ordering of input keyword file
        paths. It returns -1 when no keyword is detected.
        """

        result = c_int()
        status = self.process_func(self._handle, (c_short * len(pcm))(*pcm), byref(result))
        if status is not self.PicovoiceStatuses.SUCCESS:
            raise self._PICOVOICE_STATUS_TO_EXCEPTION[status]('Processing failed')

        keyword_index = result.value

        if self._num_keywords == 1:
            return keyword_index == 0
        else:
            return keyword_index

    def delete(self):
        """Releases resources acquired by Porcupine's library."""

        self._delete_func(self._handle)
