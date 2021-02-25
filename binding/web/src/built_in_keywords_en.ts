/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import {
  ALEXA_64,
  AMERICANO_64,
  BLUEBERRY_64,
  BUMBLEBEE_64,
  COMPUTER_64,
  GRAPEFRUIT_64,
  GRASSHOPPER_64,
  HEY_GOOGLE_64,
  HEY_SIRI_64,
  OK_GOOGLE_64,
  PICOVOICE_64,
  PORCUPINE_64,
  TERMINATOR_64,
} from './dist/en/built_in_keywords_en_64';

export enum BuiltInKeywordEn {
  Alexa = 'Alexa',
  Americano = 'Americano',
  Blueberry = 'Bluebery',
  Bumblebee = 'Bumblebee',
  Computer = 'Computer',
  Grapefruit = 'Grapefruit',
  Grasshopper = 'Grasshopper',
  HeyGoogle = 'Hey Google',
  HeySiri = 'Hey Siri',
  Jarvis = 'Jarvis',
  OkayGoogle = 'Okay Google',
  Picovoice = 'Picovoice',
  Porcupine = 'Porcupine',
  Terminator = 'Terminator',
}

export const BUILT_IN_KEYWORD_BYTES_EN = new Map<BuiltInKeywordEn, string>();
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Alexa, ALEXA_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Americano, AMERICANO_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Blueberry, BLUEBERRY_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Bumblebee, BUMBLEBEE_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Computer, COMPUTER_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Grapefruit, GRAPEFRUIT_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Grasshopper, GRASSHOPPER_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.HeyGoogle, HEY_GOOGLE_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.HeySiri, HEY_SIRI_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.OkayGoogle, OK_GOOGLE_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Picovoice, PICOVOICE_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Porcupine, PORCUPINE_64);
BUILT_IN_KEYWORD_BYTES_EN.set(BuiltInKeywordEn.Terminator, TERMINATOR_64);
