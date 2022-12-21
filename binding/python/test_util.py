#
# Copyright 2022 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import json
import os
import struct
import wave

from util import *


def load_test_data():
    data_file_path = os.path.join(os.path.dirname(__file__), "../../resources/test/test_data.json")
    with open(data_file_path) as data_file:
        json_test_data = data_file.read()
    test_data = json.loads(json_test_data)['tests']

    single_keyword_parameters = [
        (t['language'], [t['wakeword']], f"{t['wakeword'].replace(' ', '_')}.wav")
        for t in test_data['singleKeyword']]
    multiple_keywords_parameters = [
        (t['language'], t['wakewords'], t['groundTruth']) for t in test_data['multipleKeyword']]

    return single_keyword_parameters, multiple_keywords_parameters


def _append_language(s, language):
    if language == 'en':
        return s
    return "%s_%s" % (s, language)


def read_wav_file(file_name, sample_rate):
    wav_file = wave.open(file_name, mode="rb")
    channels = wav_file.getnchannels()
    num_frames = wav_file.getnframes()

    if wav_file.getframerate() != sample_rate:
        raise ValueError(
            "Audio file should have a sample rate of %d, got %d" % (sample_rate, wav_file.getframerate()))

    samples = wav_file.readframes(num_frames)
    wav_file.close()

    frames = struct.unpack('h' * num_frames * channels, samples)

    if channels == 2:
        print("Picovoice processes single-channel audio but stereo file is provided. Processing left channel only.")

    return frames[::channels]


def get_model_path_by_language(relative, language):
    model_path_subdir = _append_language('lib/common/porcupine_params', language)
    return os.path.join(os.path.dirname(__file__), relative, '%s.pv' % model_path_subdir)


def get_keyword_paths_by_language(relative, language, keywords):
    keyword_files_root = _append_language('resources/keyword_files', language)
    keyword_files_dir = os.path.join(
        os.path.dirname(__file__),
        relative,
        keyword_files_root,
        pv_keyword_files_subdir())

    res = list()
    keyword_paths = {
        keyword_file.rsplit('_')[0]: os.path.join(keyword_files_dir, keyword_file)
        for keyword_file in os.listdir(keyword_files_dir)
    }

    for keyword in keywords:
        res.append(keyword_paths[keyword])

    return res


def get_audio_file_by_language(relative, language, audio_file_name=None):
    if audio_file_name is None:
        audio_file_name = "%s.wav" % _append_language('multiple_keywords', language)
    return os.path.join(os.path.dirname(__file__), relative, 'resources/audio_samples', audio_file_name)
