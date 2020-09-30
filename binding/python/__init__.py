#
# Copyright 2020 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

from .porcupine import Porcupine
from .util import *

LIBRARY_PATH = pv_library_path('')

MODEL_PATH = pv_model_path('')

KEYWORD_FILE_PATHS = pv_keyword_file_paths('')

KEYWORDS = set(KEYWORD_FILE_PATHS.keys())


def create(library_path=None, model_path=None, keyword_file_paths=None, keywords=None, sensitivities=None):
    """
    Factory method for Porcupine Wake Word Engine.

    :param library_path: Path to Porcupine's dynamic library. If not set it will be set to the default location.
    :param model_path: Path to Porcupine's model file. If not set it will be set to the default location.
    :param keyword_file_paths: List of absolute paths to keyword files. If not set it will be populated from
    'keywords' argument.
    :param keywords: List of keywords to be used with Porcupine. The list of available (default) keywords can be
    retrieved using 'pvporcupine.KEYWORDS'.
    :param sensitivities: List of sensitivity values for each keyword. If not set '0.5' will be used for all keywords.
    :return: An instance of Porcupine Wake Word Engine.
    """

    if library_path is None:
        library_path = LIBRARY_PATH

    if model_path is None:
        model_path = MODEL_PATH

    if keyword_file_paths is None:
        if keywords is None:
            raise ValueError("'keywords' or 'keyword_file_paths' must be set")

        if all(x in KEYWORDS for x in keywords):
            keyword_file_paths = [KEYWORD_FILE_PATHS[x] for x in keywords]
        else:
            raise ValueError(
                'One or more keywords are not available by default. available keywords are:\\n%s' % ', '.join(KEYWORDS))

    if sensitivities is None:
        sensitivities = [0.5] * len(keyword_file_paths)

    if len(sensitivities) != len(keyword_file_paths):
        raise ValueError("'sensitivities' and 'keyword_file_paths' should have the same length.")

    return Porcupine(
        library_path=library_path,
        model_path=model_path,
        keyword_file_paths=keyword_file_paths,
        sensitivities=sensitivities)
