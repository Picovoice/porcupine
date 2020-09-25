import os
import shutil

import setuptools

os.system('git clean -dfx')

package_folder = os.path.join(os.path.dirname(__file__), 'pvporcupinedemo')
os.mkdir(package_folder)

shutil.copy(
    os.path.join(os.path.dirname(__file__), 'porcupine_demo_file.py'),
    os.path.join(package_folder, 'porcupine_demo_file.py'))

shutil.copy(
    os.path.join(os.path.dirname(__file__), 'porcupine_demo_mic.py'),
    os.path.join(package_folder, 'porcupine_demo_mic.py'))

shutil.copy(os.path.join(os.path.dirname(__file__), '../../LICENSE'), package_folder)

MANIFEST_IN = """
include pvporcupinedemo/LICENSE
include pvporcupinedemo/porcupine_demo_file.py
include pvporcupinedemo/porcupine_demo_mic.py
"""

with open(os.path.join(os.path.dirname(__file__), 'MANIFEST.in'), 'w') as f:
    f.write(MANIFEST_IN.strip('\n '))

LONG_DESCRIPTION = """
# [Porcupine](https://github.com/Picovoice/porcupine).   

It supports Ubuntu (x86_64), Mac (x86_64), Raspberry Pi (Zero, 1, 2, 3, 4) running Raspbian, and BeagleBone.

## Installation

```bash
sudo pip3 install pvporcupinedemo
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

### Realtime Demo

Make sure you have a working microphone connected to your device first. From commandline type the following

```bash
pvporcupinedemo_mic --keywords picovoice
```

Then say 'picovoice'. The demo processes audio steam from microphone in realtime and detects utterances of 'picovoice'

### File-Based Demo

```bash
pvporcupinedemo_file --input_audio_file_path ${INPUT_AUDIO_FILE_PATH} --keywords bumblebee
```

Replace `${INPUT_AUDIO_FILE_PATH}` with a valid path to an audio file (e.g. WAV or FLAC). The demo scans the file
for occurrences of 'bumblebee'.

In order to get more information about using demos, run them with '--help' argument or look into their GitHub page
[here](https://github.com/Picovoice/porcupine/tree/master/demo/python).
"""

setuptools.setup(
    name="pvporcupinedemo",
    version="1.8.0",
    author="Picovoice",
    author_email="hello@picovoice.ai",
    description="On-Device Speech-to-Intent engine powered by deep learning.",
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    url="https://github.com/Picovoice/porcupine",
    packages=["pvporcupinedemo"],
    install_requires=[
        "enum34==1.1.6",
        "numpy",
        "pvporcupine==1.8.2",
        "pyaudio",
        "pysoundfile>=0.9.0",
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
    entry_points=dict(
        console_scripts=[
            'pvporcupinedemo_file=pvporcupinedemo.porcupine_demo_file:main',
            'pvporcupinedemo_mic=pvporcupinedemo.porcupine_demo_mic:main',
        ],
    ),
    python_requires='>=3',
)
