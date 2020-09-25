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
TODO
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
        "pvporcupine==1.8.1",
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
            'pvporcupine_demo_file=pvporcupinedemo.porcupine_demo_file:main',
            'pvporcupine_demo_mic=pvporcupinedemo.porcupine_demo_mic:main',
        ],
    ),
    python_requires='>=3',
)
