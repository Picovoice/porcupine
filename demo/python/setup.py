import os
import shutil

import setuptools

os.system('git clean -dfx')

package_folder = os.path.join(os.path.dirname(__file__), 'pvporcupinedemo')
os.mkdir(package_folder)

shutil.copy(os.path.join(os.path.dirname(__file__), '../../LICENSE'), package_folder)

shutil.copy(
    os.path.join(os.path.dirname(__file__), 'porcupine_demo_file.py'),
    os.path.join(package_folder, 'porcupine_demo_file.py'))

shutil.copy(
    os.path.join(os.path.dirname(__file__), 'porcupine_demo_mic.py'),
    os.path.join(package_folder, 'porcupine_demo_mic.py'))

with open(os.path.join(os.path.dirname(__file__), 'MANIFEST.in'), 'w') as f:
    f.write('include pvporcupinedemo/LICENSE\n')
    f.write('include pvporcupinedemo/porcupine_demo_file.py\n')
    f.write('include pvporcupinedemo/porcupine_demo_mic.py\n')

with open(os.path.join(os.path.dirname(__file__), 'README.md'), 'r') as f:
    long_description = f.read()

with open(os.path.join(os.path.dirname(__file__), "requirements.txt"), "r") as f:
    dependencies = f.read().strip().splitlines()

setuptools.setup(
    name="pvporcupinedemo",
    version="3.0.5",
    author="Picovoice",
    author_email="hello@picovoice.ai",
    description="Porcupine wake word engine demos",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/Picovoice/porcupine",
    packages=["pvporcupinedemo"],
    install_requires=dependencies,
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
            'porcupine_demo_file=pvporcupinedemo.porcupine_demo_file:main',
            'porcupine_demo_mic=pvporcupinedemo.porcupine_demo_mic:main',
        ],
    ),
    python_requires='>=3.9',
    keywords="wake word engine, hotword detection, keyword spotting, wake word detection, voice commands",
)
