#
# Copyright 2020-2021 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import sys

from gpiozero import LED

from apa102 import APA102

COLORS_RGB = dict(
    off=(0, 0, 0),
    blue=(0, 0, 255),
    green=(0, 255, 0),
    orange=(255, 128, 0),
    pink=(255, 51, 153),
    purple=(128, 0, 128),
    red=(255, 0, 0),
    white=(255, 255, 255),
    yellow=(255, 255, 51),
)

driver = APA102(num_led=12)
power = LED(5)
power.on()


def set_color(color):
    for i in range(12):
        driver.set_pixel(i, color[0], color[1], color[2])
    driver.show()


def main():
    set_color(COLORS_RGB[sys.argv[1]])


if __name__ == '__main__':
    main()
