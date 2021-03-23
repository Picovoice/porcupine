/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import {
  FRAMBOISE_64,
  MON_CHOUCHOU_64,
  PARAPLUIE_64,
  PERROQUET_64,
  TOURNESOL_64,
} from './dist/built_in_keywords_64';

export enum BuiltInKeyword {
  Framboise = 'Framboise',
  MonChouchou = 'Mon Chouchou',
  Parapluie = 'Parapluie',
  Perroquet = 'Perroquet',
  Tournesol = 'Tournesol',
}

export const BUILT_IN_KEYWORD_BYTES = new Map<BuiltInKeyword, string>();
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Framboise, FRAMBOISE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.MonChouchou, MON_CHOUCHOU_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Parapluie, PARAPLUIE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Perroquet, PERROQUET_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Tournesol, TOURNESOL_64);
