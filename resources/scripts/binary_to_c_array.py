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

import struct
import sys

if __name__ == '__main__':
    indent = 8
    line_width = 120

    with open(sys.argv[1], 'rb') as f:
        array = f.read()

        res = list()

        array = ['0x%s' % z.hex() for z in struct.unpack('%dc' % len(array), array)]

        row = ' ' * indent
        last_x = 0
        for x in array:
            if len(row) >= line_width:
                row = row.rsplit(', ', maxsplit=1)[0] + ','
                res.append(row)
                row = ' ' * indent + last_x

            if row != ' ' * indent:
                row += ', '
            row += x

            last_x = x

        if row != ' ' * indent:
            res.append(row)
        res.append('')

        with open(sys.argv[2], 'w') as f_out:
            f_out.write('\n'.join(res))