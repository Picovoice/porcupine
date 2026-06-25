#
# Copyright 2020-2026 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

from typing import (
    Optional,
    Sequence
)

from ._porcupine import (
    list_hardware_devices,
    Porcupine
)
from ._util import (
    pv_keyword_paths,
    pv_library_path,
    pv_model_path,
    pv_train_model,
)

KEYWORD_PATHS = pv_keyword_paths()

KEYWORDS = set(KEYWORD_PATHS.keys())


def create(
        access_key: str,
        library_path: Optional[str] = None,
        model_path: Optional[str] = None,
        device: Optional[str] = None,
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
    :param device: String representation of the device (e.g., CPU or GPU) to use. If set to `best`, the most
    suitable device is selected automatically. If set to `gpu`, the engine uses the first available GPU device.
    To select a specific GPU device, set this argument to `gpu:${GPU_INDEX}`, where `${GPU_INDEX}` is the index
    of the target GPU. If set to`cpu`, the engine will run on the CPU with the default number of threads. To
    specify the number of threads, set this argument to `cpu:${NUM_THREADS}`, where `${NUM_THREADS}` is the
    desired number of threads.
    :param keyword_paths: Absolute paths to keyword model files. If not set it will be populated from `keywords`
    argument.
    :param keywords: List of keywords (phrases) for detection. The list of available (default) keywords can be
    retrieved using `pvporcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
    :param sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher
    sensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.
    :return: An instance of Porcupine wake word engine.
    """

    if library_path is None:
        library_path = pv_library_path()

    if model_path is None:
        model_path = pv_model_path()

    if device is None:
        device = "best"

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
        device=device,
        keyword_paths=keyword_paths,
        sensitivities=sensitivities)


def available_devices(library_path: Optional[str] = None) -> Sequence[str]:
    """
    Lists all available devices that Porcupine can use for inference. Each entry in the list can be the `device`
    argument of `.create` factory method or `Porcupine` constructor.

    :param library_path: Absolute path to Porcupine's dynamic library.
    If not set it will be set to the default location.

    :return: List of all available devices that Porcupine can use for inference.
    """

    if library_path is None:
        library_path = pv_library_path()

    return list_hardware_devices(library_path=library_path)


def train_wake_word_from_phrase(
        access_key: str,
        output_path: str,
        language: str,
        phrase: str,
        platform: Optional[str] = None) -> None:
    """
    Trains a wake word model given a phrase.

    :param access_key: AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
    :param output_path: Absolute path to file where the trained model will be saved.
    :param language: Two character language code for the model (e.g. 'en', 'fr').
    Check https://picovoice.ai/docs/model-api/porcupine/ for supported languages.
    :param phrase: The phrase to create a wake word from.
    :param platform: The platform running this code will be used. If None, the default(current) platform is used.
    Check https://picovoice.ai/docs/model-api/porcupine/ for supported platforms.
    """

    pv_train_model(
        access_key=access_key,
        output_path=output_path,
        language=language,
        phrase=phrase,
        platform=platform)


__all__ = [
    'available_devices',
    'create',
    'KEYWORDS',
    'KEYWORD_PATHS',
    'train_wake_word_from_phrase',
]
