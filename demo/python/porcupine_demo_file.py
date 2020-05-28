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

import argparse
import os
import sys

import soundfile

sys.path.append(os.path.join(os.path.dirname(__file__), '../../binding/python'))
sys.path.append(os.path.join(os.path.dirname(__file__), '../../resources/util/python'))

from util import *
from porcupine import Porcupine


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


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--input_audio_file_path',
        help='absolute path to input audio file',
        required=True)

    parser.add_argument(
        '--keywords',
        nargs='+',
        help='list of default keywords',
        choices=KEYWORDS)

    parser.add_argument(
        '--keyword_file_paths',
        nargs='+',
        help='list of absolute paths to keyword files')

    parser.add_argument('--library_path', help="absolute path to Porcupine's dynamic library", default=LIBRARY_PATH)

    parser.add_argument('--model_file_path', help='absolute path to model parameter file', default=MODEL_FILE_PATH)

    parser.add_argument('--sensitivity', help='detection sensitivity [0, 1]', default=0.5)

    args = parser.parse_args()

    if args.keyword_file_paths is None:
        if args.keywords is None:
            raise ValueError('either --keywords or --keyword_file_paths must be set')

        keyword_file_paths = [KEYWORD_FILE_PATHS[x] for x in args.keywords]
    else:
        keyword_file_paths = args.keyword_file_paths

    _run(
        input_audio_file_path=args.input_audio_file_path,
        library_path=args.library_path,
        model_file_path=args.model_file_path,
        keyword_file_paths=keyword_file_paths,
        sensitivity=float(args.sensitivity))


if __name__ == '__main__':
    main()
