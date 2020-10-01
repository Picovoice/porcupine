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
import struct
from datetime import datetime
from threading import Thread

import numpy as np
import pvporcupine
import pyaudio
import soundfile


class PorcupineDemo(Thread):
    """
    Microphone Demo for Porcupine Wake Word Engine. It creates an input audio stream from a microphone, monitors it, and
    upon detecting the specified wake word(s) prints the detection time and wake word on console. It optionally saves
    the recorded audio into a file for further review.
    """

    def __init__(
            self,
            library_path,
            model_path,
            keyword_file_paths,
            sensitivities,
            input_device_index=None,
            output_path=None):

        """
        Constructor.

        :param library_path: Absolute path to Porcupine's dynamic library.
        :param model_path: Absolute path to the model parameter file.
        :param keyword_file_paths: Absolute paths to keyword files.
        :param sensitivities: Sensitivity parameter for each wake word.
        :param input_device_index: Optional argument. If provided, audio is recorded from this input device. Otherwise,
        the default audio input device is used.
        :param output_path: If provided recorded audio will be stored in this location at the end of the run.
        """

        super(PorcupineDemo, self).__init__()

        self._library_path = library_path
        self._model_path = model_path
        self._keyword_file_paths = keyword_file_paths
        self._sensitivities = sensitivities
        self._input_device_index = input_device_index

        self._output_path = output_path
        if self._output_path is not None:
            self._recorded_frames = []

    def run(self):
        """
         Creates an input audio stream, instantiates an instance of Porcupine object, and monitors the audio stream for
         occurrences of the wake word(s). It prints the time of detection for each occurrence and the wake word.
         """

        keyword_names = list()
        for x in self._keyword_file_paths:
            keyword_names.append(os.path.basename(x).replace('.ppn', '').split('_')[0])

        porcupine = None
        pa = None
        audio_stream = None
        try:
            porcupine = pvporcupine.create(
                library_path=self._library_path,
                model_path=self._model_path,
                keyword_file_paths=self._keyword_file_paths,
                sensitivities=self._sensitivities)

            pa = pyaudio.PyAudio()

            audio_stream = pa.open(
                rate=porcupine.sample_rate,
                channels=1,
                format=pyaudio.paInt16,
                input=True,
                frames_per_buffer=porcupine.frame_length,
                input_device_index=self._input_device_index)

            print('Listening ...')
            for keyword_name, sensitivity in zip(keyword_names, self._sensitivities):
                print('- %s (sensitivity: %.2f)' % (keyword_name, sensitivity))

            while True:
                pcm = audio_stream.read(porcupine.frame_length)
                pcm = struct.unpack_from("h" * porcupine.frame_length, pcm)

                if self._output_path is not None:
                    self._recorded_frames.append(pcm)

                result = porcupine.process(pcm)
                if result >= 0:
                    print('[%s] detected %s' % (str(datetime.now()), keyword_names[result]))

        except KeyboardInterrupt:
            print('Stopping ...')
        finally:
            if porcupine is not None:
                porcupine.delete()

            if audio_stream is not None:
                audio_stream.close()

            if pa is not None:
                pa.terminate()

            if self._output_path is not None and len(self._recorded_frames) > 0:
                recorded_audio = np.concatenate(self._recorded_frames, axis=0).astype(np.int16)
                soundfile.write(self._output_path, recorded_audio, samplerate=porcupine.sample_rate, subtype='PCM_16')

    @classmethod
    def show_audio_devices(cls):
        fields = ('index', 'name', 'defaultSampleRate', 'maxInputChannels')

        pa = pyaudio.PyAudio()

        for i in range(pa.get_device_count()):
            info = pa.get_device_info_by_index(i)
            print(', '.join("'%s': '%s'" % (k, str(info[k])) for k in fields))

        pa.terminate()


def main():
    parser = argparse.ArgumentParser()

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

    parser.add_argument('--audio_device_index', help='Index of input audio device.', type=int, default=None)

    parser.add_argument('--output_path', help='Absolute path to recorded audio for debugging.')

    parser.add_argument('--show_audio_devices', action='store_true')

    args = parser.parse_args()

    if args.show_audio_devices:
        PorcupineDemo.show_audio_devices()
    else:
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

        PorcupineDemo(
            library_path=args.library_path,
            model_path=args.model_path,
            keyword_file_paths=keyword_file_paths,
            sensitivities=args.sensitivities,
            output_path=args.output_path,
            input_device_index=args.audio_device_index).run()


if __name__ == '__main__':
    main()
