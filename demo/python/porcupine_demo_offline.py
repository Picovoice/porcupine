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

import argparse
import os
import platform
import sys

import soundfile

sys.path.append(os.path.join(os.path.dirname(__file__), '../../binding/python'))

from porcupine import Porcupine


def _default_library_path():
    system = platform.system()
    machine = platform.machine()

    if system == 'Darwin':
        return os.path.join(os.path.dirname(__file__), '../../lib/mac/%s/libpv_porcupine.dylib' % machine)
    elif system == 'Linux':
        if machine == 'x86_64' or machine == 'i386':
            return os.path.join(os.path.dirname(__file__), '../../lib/linux/%s/libpv_porcupine.so' % machine)
        else:
            raise Exception('cannot autodetect the binary type. Please enter the path to the shared object using --library_path command line argument.')
    elif system == 'Windows':
        if platform.architecture()[0] == '32bit':
            return os.path.join(os.path.dirname(__file__), '..\\..\\lib\\windows\\i686\\libpv_porcupine.dll')
        else:
            return os.path.join(os.path.dirname(__file__), '..\\..\\lib\\windows\\amd64\\libpv_porcupine.dll')

    raise NotImplementedError('Porcupine is not supported on %s/%s yet!' % (system, machine))


def _run(input_audio_file_path, library_path, model_file_path, keyword_file_paths, sensitivity):
    """
    Monitors an input audio file for occurrences of keywords for which keyword files are provided and prints their
    occurrence time (in seconds from start of file).

    :param input_audio_file_path: Absolute path to input audio file. The file should have a sample rate of 16000 and
    be single-channel.
    :param library_path: Absolute path to Porcupine's dynamic library.
    :param model_file_path: Absolute path to the model parameter file.
    :param keyword_file_paths: List of absolute paths to keyword files.
    :param sensitivity: Sensitivity parameter. For more information refer to 'include/pv_porcupine.h'. It uses the
    same sensitivity value for all keywords.
    :return:
    """

    num_keywords = len(keyword_file_paths)

    porcupine = Porcupine(
        library_path=library_path,
        model_file_path=model_file_path,
        keyword_file_paths=keyword_file_paths,
        sensitivities=[sensitivity] * num_keywords)

    def _frame_index_to_sec(frame_index):
        return float(frame_index * porcupine.frame_length) / float(porcupine.sample_rate)

    audio, sample_rate = soundfile.read(input_audio_file_path, dtype='int16')
    assert sample_rate == porcupine.sample_rate

    num_frames = len(audio) // porcupine.frame_length
    for i in range(num_frames):
        frame = audio[i * porcupine.frame_length:(i + 1) * porcupine.frame_length]
        result = porcupine.process(frame)
        if num_keywords == 1 and result:
            print('detected keyword at time %f' % _frame_index_to_sec(i))
        elif num_keywords > 1 and result >= 0:
            print('detected keyword index %d at time %f' % (result, _frame_index_to_sec(i)))

    porcupine.delete()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('--input_audio_file_path', help='Absolute path to input audio file', type=str, required=True)

    parser.add_argument('--keyword_file_paths', help='comma-separated absolute paths to keyword files', type=str, required=True)

    parser.add_argument(
        '--library_path',
        help="absolute path to Porcupine's dynamic library",
        type=str)

    parser.add_argument(
        '--model_file_path',
        help='absolute path to model parameter file',
        type=str,
        default=os.path.join(os.path.dirname(__file__), '../../lib/common/porcupine_params.pv'))

    parser.add_argument('--sensitivity', help='detection sensitivity [0, 1]', default=0.5)

    args = parser.parse_args()

    _run(
        input_audio_file_path=args.input_audio_file_path,
        library_path=args.library_path if args.library_path is not None else _default_library_path(),
        model_file_path=args.model_file_path,
        keyword_file_paths=[x.strip() for x in args.keyword_file_paths.split(',')],
        sensitivity=float(args.sensitivity))
