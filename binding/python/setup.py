import os
import shutil

import setuptools

os.system('git clean -dfx')

package_folder = os.path.join(os.path.dirname(__file__), 'pvporcupine')
os.mkdir(package_folder)

os.makedirs(os.path.join(package_folder, 'binding/python'))
shutil.copy(
    os.path.join(os.path.dirname(__file__), 'porcupine.py'),
    os.path.join(package_folder, 'binding/python/porcupine.py'))

platforms = ('beaglebone', 'linux', 'mac', 'raspberry-pi', 'windows')

os.mkdir(os.path.join(package_folder, 'lib'))
for platform in ('common',) + platforms:
    shutil.copytree(
        os.path.join(os.path.dirname(__file__), '../../lib', platform),
        os.path.join(package_folder, 'lib', platform))

for platform in platforms:
    shutil.copytree(
        os.path.join(os.path.dirname(__file__), '../../resources/keyword_files', platform),
        os.path.join(package_folder, 'resources/keyword_files', platform))

os.makedirs(os.path.join(package_folder, 'resources/util/python'))
shutil.copy(
    os.path.join(os.path.dirname(__file__), '../../resources/util/python/__init__.py'),
    os.path.join(package_folder, 'resources/util/python'))
shutil.copy(
    os.path.join(os.path.dirname(__file__), '../../resources/util/python/util.py'),
    os.path.join(package_folder, 'resources/util/python'))

shutil.copy(os.path.join(os.path.dirname(__file__), '../../LICENSE'), package_folder)

INIT_SCRIPT = """
from .binding.python.porcupine import Porcupine
from .resources.util.python import *


def create(library_path=None, model_file_path=None, keyword_file_paths=None, keywords=None, sensitivities=None):
    \"""
    Factory method for Porcupine.

    :param library_path: Path to Porcupine's dynamic library. If not set it will be set to the default location.
    :param model_file_path: Path to Porcupine's model parameters file. If not set it will be set to the default
    location.
    :param keyword_file_paths: List of absolute paths to keywords files. If not set it will be populated from
    'keywords' argument.
    :param keywords: List of keywords to be used with Porcupine. The list of available (default) keywords can be
    retrieved using 'pvporcupine.KEYWORDS'.
    :param sensitivities: List of sensitivity values for each keyword. If not set '0.5' will be used for all keywords.
    :return: An instance of Porcupine wake word engine.
    \"""

    if library_path is None:
        library_path = LIBRARY_PATH

    if model_file_path is None:
        model_file_path = MODEL_FILE_PATH

    if keyword_file_paths is None:
        if keywords is None:
            raise ValueError("'keywords' or 'keyword_file_paths' must be set")

        if all(x in KEYWORDS for x in keywords):
            keyword_file_paths = [KEYWORD_FILE_PATHS[x] for x in keywords]
        else:
            raise ValueError(
                'one or more keywords are not available by default. available keywords are:\\n%s' % ', '.join(KEYWORDS))

    if sensitivities is None:
        sensitivities = [0.5] * len(keyword_file_paths)

    if len(sensitivities) != len(keyword_file_paths):
        raise ValueError("'sensitivities' and 'keyword_file_paths' should have the same length")

    return Porcupine(
        library_path=library_path,
        model_file_path=model_file_path,
        keyword_file_paths=keyword_file_paths,
        sensitivities=sensitivities)
"""

with open(os.path.join(package_folder, '__init__.py'), 'w') as f:
    f.write(INIT_SCRIPT.strip('\n '))
    f.write('\n')

MANIFEST_IN = """
include pvporcupine/LICENSE
include pvporcupine/binding/python/porcupine.py
include pvporcupine/lib/common/porcupine_params.pv
include pvporcupine/lib/beaglebone/libpv_porcupine.so
include pvporcupine/lib/linux/x86_64/libpv_porcupine.so
include pvporcupine/lib/mac/x86_64/libpv_porcupine.dylib
include pvporcupine/lib/raspberry-pi/arm11/libpv_porcupine.so
include pvporcupine/lib/raspberry-pi/cortex-a7/libpv_porcupine.so
include pvporcupine/lib/raspberry-pi/cortex-a53/libpv_porcupine.so
include pvporcupine/lib/raspberry-pi/cortex-a72/libpv_porcupine.so
include pvporcupine/lib/windows/amd64/libpv_porcupine.dll
recursive-include pvporcupine/resources/keyword_files/beaglebone *
recursive-include pvporcupine/resources/keyword_files/linux *
recursive-include pvporcupine/resources/keyword_files/mac/ *
recursive-include pvporcupine/resources/keyword_files/raspberry-pi *
recursive-include pvporcupine/resources/keyword_files/windows *
recursive-include pvporcupine/resources/util/python *
"""

with open(os.path.join(os.path.dirname(__file__), 'MANIFEST.in'), 'w') as f:
    f.write(MANIFEST_IN.strip('\n '))

LONG_DESCRIPTION = """
# [Porcupine](https://github.com/Picovoice/porcupine).   

It supports Ubuntu (x86_64), Mac (x86_64), Raspberry Pi (Zero, 1, 2, 3, 4) running Raspbian, and BeagleBone.

## Installation

```bash
sudo pip3 install pvporcupine
```

If it fails to install PyAudio, you can do the following for Debian/Ubuntu as referenced in the installation guide
of [PyAudio](https://people.csail.mit.edu/hubert/pyaudio/). 

Install PyAudio  

```bash
sudo apt-get install python3-pyaudio
```

If the above fails then first run the following

```bash
sudo apt-get install portaudio19-dev
sudo apt-get install python3-all-dev
```

## Usage

You can create an instance of Porcupine engine for use within your application using the factory method provided below

```python
import pvporcupine

pvporcupine.create(keywords=pvporcupine.KEYWORDS)
```

`pvporcupine.KEYWORDS` is the set of default keyword files that ships with the PIP package across all platforms. In
order to use your own keyword file you can instantiate the object as follows:

```python
import pvporcupine

keyword_file_1_path = ...
keyword_file_2_path = ...
keyword_file_3_path = ...

pvporcupine.create(keyword_file_paths=[keyword_file_1_path, keyword_file_2_path, keyword_file_3_path])
```
"""

setuptools.setup(
    name="pvporcupine",
    version="1.8.2",
    author="Picovoice",
    author_email="hello@picovoice.ai",
    description="On-Device wake word detection powered by deep learning.",
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    url="https://github.com/Picovoice/porcupine",
    packages=["pvporcupine"],
    install_requires=[
        "enum34==1.1.6",
        "numpy",
    ],
    include_package_data=True,
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Topic :: Multimedia :: Sound/Audio :: Speech"
    ],
    python_requires='>=3',
)
