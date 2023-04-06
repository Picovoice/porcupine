#
# Copyright 2023 Picovoice Inc.
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


def get_lib_ext(platform):
    if platform == "windows":
        return "dll"
    elif platform == "mac":
        return "dylib"
    else:
        return "so"


def append_language(s, language):
    if language == 'en':
        return s
    return "%s_%s" % (s, language)


def load_test_data():
    data_file_path = os.path.join(os.path.dirname(__file__), "../../../resources/.test/test_data.json")
    with open(data_file_path) as data_file:
        json_test_data = data_file.read()
    test_data = json.loads(json_test_data)['tests']

    single_keyword_parameters = [
        (t['language'], t['wakeword'], f"{t['wakeword'].replace(' ', '_')}.wav")
        for t in test_data['singleKeyword']]

    return single_keyword_parameters
