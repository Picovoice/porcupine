#
# Copyright 2019-2022 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import os
import shutil

import setuptools

os.system('git clean -dfx')

package_folder = os.path.join(os.path.dirname(__file__), 'pvporcupine')
os.mkdir(package_folder)

shutil.copy(os.path.join(os.path.dirname(__file__), '../../LICENSE'), package_folder)

shutil.copy(os.path.join(os.path.dirname(__file__), '__init__.py'), os.path.join(package_folder, '__init__.py'))
shutil.copy(os.path.join(os.path.dirname(__file__), 'porcupine.py'), os.path.join(package_folder, 'porcupine.py'))
shutil.copy(os.path.join(os.path.dirname(__file__), 'util.py'), os.path.join(package_folder, 'util.py'))

platforms = ('beaglebone', 'jetson', 'linux', 'mac', 'raspberry-pi', 'windows')

os.mkdir(os.path.join(package_folder, 'lib'))
for platform in ('common',) + platforms:
    shutil.copytree(
        os.path.join(os.path.dirname(__file__), '../../lib', platform),
        os.path.join(package_folder, 'lib', platform))

for platform in platforms:
    shutil.copytree(
        os.path.join(os.path.dirname(__file__), '../../resources/keyword_files', platform),
        os.path.join(package_folder, 'resources/keyword_files', platform))

MANIFEST_IN = """
include pvporcupine/LICENSE
include pvporcupine/__init__.py
include pvporcupine/porcupine.py
include pvporcupine/util.py
include pvporcupine/lib/common/porcupine_params.pv
include pvporcupine/lib/beaglebone/libpv_porcupine.so
recursive-include pvporcupine/lib/jetson *
include pvporcupine/lib/linux/x86_64/libpv_porcupine.so
include pvporcupine/lib/mac/x86_64/libpv_porcupine.dylib
include pvporcupine/lib/mac/arm64/libpv_porcupine.dylib
recursive-include pvporcupine/lib/raspberry-pi *
include pvporcupine/lib/windows/amd64/libpv_porcupine.dll
recursive-include pvporcupine/resources/keyword_files/beaglebone *
recursive-include pvporcupine/resources/keyword_files/jetson *
recursive-include pvporcupine/resources/keyword_files/linux *
recursive-include pvporcupine/resources/keyword_files/mac/ *
recursive-include pvporcupine/resources/keyword_files/raspberry-pi *
recursive-include pvporcupine/resources/keyword_files/windows *
"""

with open(os.path.join(os.path.dirname(__file__), 'MANIFEST.in'), 'w') as f:
    f.write(MANIFEST_IN.strip('\n '))

with open(os.path.join(os.path.dirname(__file__), 'README.md'), 'r') as f:
    long_description = f.read()

setuptools.setup(
    name="pvporcupine",
    version="2.1.4",
    author="Picovoice",
    author_email="hello@picovoice.ai",
    description="Porcupine wake word engine.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/Picovoice/porcupine",
    packages=["pvporcupine"],
    include_package_data=True,
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Topic :: Multimedia :: Sound/Audio :: Speech"
    ],
    python_requires='>=3.5',
    keywords="wake word engine, hotword detection, keyword spotting, wake word detection, voice commands",
)
