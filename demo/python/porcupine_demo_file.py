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

import argparse
import os

import pvporcupine
import soundfile


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--input_audio_path', help='Absolute path to input audio file.', required=True)

    parser.add_argument('--keywords', nargs='+', help='Default keywords.', choices=pvporcupine.KEYWORDS)

    parser.add_argument('--keyword_file_paths', nargs='+', help='Absolute paths to keyword files.')

    parser.add_argument('--library_path', help='Absolute path to dynamic library.', default=pvporcupine.LIBRARY_PATH)

    parser.add_argument('--model_path', help='Absolute path to model parameter file.', default=pvporcupine.MODEL_PATH)

    parser.add_argument(
        '--sensitivities',
        nargs='+',
        help='Detection sensitivities. Should be within [0, 1].',
        type=float,
        default=None)

    args = parser.parse_args()

    if args.keyword_file_paths is None:
        if args.keywords is None:
            raise ValueError("Either '--keywords' or '--keyword_file_paths' must be set")

        keyword_file_paths = [pvporcupine.KEYWORD_FILE_PATHS[x] for x in args.keywords]
    else:
        keyword_file_paths = args.keyword_file_paths

    if args.sensitivities is None:
        args.sensitivities = [0.5] * len(keyword_file_paths)

    if len(keyword_file_paths) != len(args.sensitivities):
        raise ValueError('Incorrect number of sensitivities.')

    porcupine = pvporcupine.create(
        library_path=args.library_path,
        model_path=args.model_path,
        keyword_file_paths=keyword_file_paths,
        sensitivities=args.sensitivities)

    audio, sample_rate = soundfile.read(args.input_audio_path, dtype='int16')
    if audio.ndim == 2:
        print("Picovoice processes single-channel audio but stereo file is provided. Processing left channel only.")
        audio = audio[0, :]
    if sample_rate != porcupine.sample_rate:
        raise ValueError("Audio file should have a sample rate of %d. got %d" % (porcupine.sample_rate, sample_rate))

    keyword_names = list()
    for x in keyword_file_paths:
        keyword_names.append(os.path.basename(x).replace('.ppn', '').split('_')[0])

    num_frames = len(audio) // porcupine.frame_length
    for i in range(num_frames):
        frame = audio[i * porcupine.frame_length:(i + 1) * porcupine.frame_length]
        result = porcupine.process(frame)
        if result >= 0:
            print(
                "Detected '%s' at time %.2f" %
                (keyword_names[result], float(i * porcupine.frame_length) / float(porcupine.sample_rate)))

    porcupine.delete()


if __name__ == '__main__':
    main()
