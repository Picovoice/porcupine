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
  JARVIS_64,
  GRAPEFRUIT_64,
  GRASSHOPPER_64,
  HEY_GOOGLE_64,
  HEY_SIRI_64,
  OK_GOOGLE_64,
  PICOVOICE_64,
  PORCUPINE_64,
  TERMINATOR_64,
} from './dist/built_in_keywords_64';

export enum BuiltInKeyword {
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

export const BUILT_IN_KEYWORD_BYTES = new Map<BuiltInKeyword, string>();
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Alexa, ALEXA_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Americano, AMERICANO_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Blueberry, BLUEBERRY_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Bumblebee, BUMBLEBEE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Computer, COMPUTER_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Jarvis, JARVIS_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Grapefruit, GRAPEFRUIT_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Grasshopper, GRASSHOPPER_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.HeyGoogle, HEY_GOOGLE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.HeySiri, HEY_SIRI_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.OkayGoogle, OK_GOOGLE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Picovoice, PICOVOICE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Porcupine, PORCUPINE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Terminator, TERMINATOR_64);
