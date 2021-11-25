#
# Copyright 2018-2021 Picovoice Inc.
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

import pvporcupine
import soundfile


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--access_key',
                        help='AccessKey obtained from Picovoice Console (https://picovoice.ai/console/)',
                        required=True)

    parser.add_argument('--input_audio_path', help='Absolute path to input audio file.', required=True)

    parser.add_argument(
        '--keywords',
        nargs='+',
        help='List of default keywords for detection. Available keywords: %s' % ', '.join(sorted(pvporcupine.KEYWORDS)),
        choices=sorted(list(pvporcupine.KEYWORDS)),
        metavar='')

    parser.add_argument(
        '--keyword_paths',
        nargs='+',
        help="Absolute paths to keyword model files. If not set it will be populated from `--keywords` argument")

    parser.add_argument('--library_path', help='Absolute path to dynamic library.', default=pvporcupine.LIBRARY_PATH)

    parser.add_argument(
        '--model_path',
        help='Absolute path to the file containing model parameters.',
        default=pvporcupine.MODEL_PATH)

    parser.add_argument(
        '--sensitivities',
        nargs='+',
        help="Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher " +
             "sensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 " +
             "will be used.",
        type=float,
        default=None)

    args = parser.parse_args()

    if args.keyword_paths is None:
        if args.keywords is None:
            raise ValueError("Either `--keywords` or `--keyword_paths` must be set.")

        keyword_paths = [pvporcupine.KEYWORD_PATHS[x] for x in args.keywords]
    else:
        keyword_paths = args.keyword_paths

    if args.sensitivities is None:
        args.sensitivities = [0.5] * len(keyword_paths)

    if len(keyword_paths) != len(args.sensitivities):
        raise ValueError('Number of keywords does not match the number of sensitivities.')

    porcupine = pvporcupine.create(
        access_key=args.access_key,
        library_path=args.library_path,
        model_path=args.model_path,
        keyword_paths=keyword_paths,
        sensitivities=args.sensitivities)

    audio, sample_rate = soundfile.read(args.input_audio_path, dtype='int16')
    if audio.ndim == 2:
        print("Picovoice processes single-channel audio but stereo file is provided. Processing left channel only.")
        audio = audio[0, :]
    if sample_rate != porcupine.sample_rate:
        raise ValueError("Audio file should have a sample rate of %d. got %d" % (porcupine.sample_rate, sample_rate))

    keywords = list()
    for x in keyword_paths:
        keyword_phrase_part = os.path.basename(x).replace('.ppn', '').split('_')
        if len(keyword_phrase_part) > 6:
            keywords.append(' '.join(keyword_phrase_part[0:-6]))
        else:
            keywords.append(keyword_phrase_part[0])

    num_frames = len(audio) // porcupine.frame_length
    for i in range(num_frames):
        frame = audio[i * porcupine.frame_length:(i + 1) * porcupine.frame_length]
        result = porcupine.process(frame)
        if result >= 0:
            print(
                "Detected '%s' at %.2f sec" %
                (keywords[result], float(i * porcupine.frame_length) / float(porcupine.sample_rate)))

    porcupine.delete()


if __name__ == '__main__':
    main()
