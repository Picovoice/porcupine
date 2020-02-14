import setuptools
import shutil
import os

LONG_DESCRIPTION = \
    """
    #[Porcupine](https://github.com/Picovoice/porcupine).   

    It supports Linux (x86_64), Mac, Raspberry Pi (Zero, 1, 2, 3, 4), and BeagleBone.

    ## Installation

    ```bash
    pip install pvporcupine
    ```

    If it fails to install PyAudio, you can do the following for Debian/Ubuntu as referenced in the installation guide
    of [PyAudio](https://people.csail.mit.edu/hubert/pyaudio/). 

    Install PyAudio  

    ```bash
    sudo apt-get install python-pyaudio python3-pyaudio
    ```

    If the above fails then first run the following

    ```bash
    sudo apt-get install portaudio19-dev
    sudo apt-get install python-all-dev python3-all-dev
    ```

    ## Usage

    ### Realtime Demo

    Make sure you have a working microphone connected to your device first. From commandline type the following

    ```bash
    pvporcupine_mic --keywords picovoice
    ```

    Then say 'picovoice'. The demo processes audio steam from microphone in realtime and detects utterances of 'picovoice'

    ### File-Based Demo

    ```bash
    pvporcupine_file --input_audio_file_path ${INPUT_AUDIO_FILE_PATH} --keywords bumblebee
    ```

    Replace `${INPUT_AUDIO_FILE_PATH}` with a valid path to an audio file (e.g. WAV or FLAC). The demo scans the file
    for occurrences of 'bumblebee'.

    In order to get more information about using demos, run them with '--help' argument or look into their GitHub page
    [here](https://github.com/Picovoice/porcupine/tree/master/demo/python).

    ### Porcupine Class
    
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

for x in ('build', 'dist', 'pvporcupine.egg-info'):
    x_path = os.path.join(os.path.dirname(__file__), x)
    if os.path.isdir(x_path):
        shutil.rmtree(x_path)

setuptools.setup(
    name="pvporcupine",
    version="1.7.0",
    author="Picovoice",
    author_email="hello@picovoice.ai",
    description="On-device wake word detection powered by deep learning.",
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    url="https://github.com/Picovoice/porcupine",
    package_dir={"pvporcupine": ""},
    packages=["pvporcupine"],
    install_requires=[
        "pysoundfile>=0.9.0",
        "enum34==1.1.6",
        "numpy",
        "pyaudio",
    ],
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
    ],
    entry_points=dict(
        console_scripts=[
            'pvporcupine_mic=pvporcupine.demo.python.porcupine_demo_mic:main',
            'pvporcupine_file=pvporcupine.demo.python.porcupine_demo_file:main'
        ],
    ),
    python_requires='>=3',
)
