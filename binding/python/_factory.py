#
# Copyright 2020-2023 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

from typing import Optional, Sequence

from ._porcupine import Porcupine
from ._util import *

KEYWORD_PATHS = pv_keyword_paths('')

KEYWORDS = set(KEYWORD_PATHS.keys())


def create(
        access_key,
        library_path: Optional[str] = None,
        model_path: Optional[str] = None,
        keyword_paths: Optional[Sequence[str]] = None,
        keywords: Optional[Sequence[str]] = None,
        sensitivities: Optional[Sequence[float]] = None) -> Porcupine:
    """
    Factory method for Porcupine wake word engine.

    :param access_key: AccessKey obtained from Picovoice Console.
    :param library_path: Absolute path to Porcupine's dynamic library. If not set it will be set to the default
    location.
    :param model_path: Absolute path to the file containing model parameters. If not set it will be set to the default
    location.
    :param keyword_paths: Absolute paths to keyword model files. If not set it will be populated from `keywords`
    argument.
    :param keywords: List of keywords (phrases) for detection. The list of available (default) keywords can be
    retrieved using `pvporcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
    :param sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher
    sensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.
    :return: An instance of Porcupine wake word engine.
    """

    if library_path is None:
        library_path = pv_library_path('')

    if model_path is None:
        model_path = pv_model_path('')

    if keyword_paths is None:
        if keywords is None:
            raise ValueError("Either `keywords` or `keyword_paths` must be set.")

        if all(x in KEYWORDS for x in keywords):
            keyword_paths = [KEYWORD_PATHS[x] for x in keywords]
        else:
            raise ValueError(
                'One or more keywords are not available by default. Available default keywords are:\\n%s' %
                ', '.join(KEYWORDS))

    if sensitivities is None:
        sensitivities = [0.5] * len(keyword_paths)

    if len(sensitivities) != len(keyword_paths):
        raise ValueError("Number of keywords does not match the number of sensitivities.")

    return Porcupine(
        access_key=access_key,
        library_path=library_path,
        model_path=model_path,
        keyword_paths=keyword_paths,
        sensitivities=sensitivities)


__all__ = [
    'create',
    'KEYWORDS',
    'KEYWORD_PATHS',
]
