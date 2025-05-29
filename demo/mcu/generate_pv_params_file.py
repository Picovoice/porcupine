#
# Copyright 2020-2025 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import os
import struct

HEADER = """
/*
    Copyright 2020-2025 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#ifndef PV_PARAMS_H
#define PV_PARAMS_H

#include <stdint.h>

"""

FOOTER = """
#endif // PV_PARAMS_H

"""

LANGUAGE_CODE_TO_NAME = {
    "de": "german",
    "en": "english",
    "es": "spanish",
    "fr": "french",
    "it": "italian",
    "ja": "japanese",
    "ko": "korean",
    "pt": "portuguese",
    "zh": "mandarin",
}


def generate_pv_params(ppn_files, header_file_folders):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    repo_dir = os.path.join(script_dir, "../..")

    for header_file_path in header_file_folders:
        header_file = os.path.join(header_file_path, "pv_params.h")
        with open(os.path.join(script_dir, header_file), "w") as f_out:
            f_out.write(HEADER)

            for language, keywords in ppn_files.items():
                if language == "en":
                    ppn_dir = os.path.join(repo_dir, "resources/keyword_files/cortexm")
                else:
                    ppn_dir = os.path.join(repo_dir, f"resources/keyword_files_{language}/cortexm")

                f_out.write(f"#if defined(__PV_LANGUAGE_{LANGUAGE_CODE_TO_NAME[language].upper()}__)\n\n")
                for index, keyword in enumerate(keywords):
                    keyword_file_path = os.path.join(ppn_dir, keyword + "_cortexm.ppn")
                    ppn_c_array = ppn_to_c_array(keyword_file_path)
                    f_out.write(f"// Wake-word = {keyword}\n")
                    if index == 0:
                        f_out.write(
                            "static const uint8_t DEFAULT_KEYWORD_ARRAY[] " "__attribute__ ((aligned (16))) = {\n"
                        )
                    else:
                        f_out.write(
                            f"static const uint8_t {keyword.upper()}_KEYWORD_ARRAY[] "
                            "__attribute__ ((aligned (16))) = {\n"
                        )
                    f_out.write("\n".join(ppn_c_array))
                    f_out.write("};\n\n")

                f_out.write("#endif\n\n")

            f_out.write(FOOTER)


def ppn_to_c_array(ppn_file_path):
    indent = 8
    line_width = 120
    with open(ppn_file_path, "rb") as f:
        array = f.read()
        res = list()
        array = ["0x%s" % z.hex() for z in struct.unpack("%dc" % len(array), array)]
        row = " " * indent
        last_x = 0
        for x in array:
            if len(row) >= line_width:
                row = row.rsplit(", ", maxsplit=1)[0] + ","
                res.append(row)
                row = " " * indent + last_x
            if row != " " * indent:
                row += ", "
            row += x
            last_x = x
        if row != " " * indent:
            res.append(row)
        res.append("")
        return res


if __name__ == "__main__":
    wake_words = {
        "de": ("hey computer",),
        "en": (
            "porcupine",
            "picovoice",
            "bumblebee",
            "alexa",
        ),
        "es": ("hola computadora",),
        "fr": ("bonjour ordinateur",),
        "it": ("ciao computer",),
        "ja": ("こんにちは コンピューター",),
        "ko": ("안녕 컴퓨터",),
        "pt": ("olá computador",),
        "zh": ("你好电脑",),
    }
    include_folders = [
        "stm32f411/stm32f411e-disco/Inc/",
    ]

    generate_pv_params(wake_words, include_folders)
