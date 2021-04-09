/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

export enum BuiltInKeyword {
  Ananas = 'Ananas',
  Heuschrecke = 'Heuschrecke',
  Himbeere = 'Himbeere',
  Leguan = 'Leguan',
  Stachelschwein = 'Stachelschwein',
}

import {
  ANANAS_64,
  HEUSCHRECKE_64,
  HIMBEERE_64,
  LEGUAN_64,
  STACHELSCHWEIN_64,
} from './dist/built_in_keywords_64';

export const BUILT_IN_KEYWORD_BYTES = new Map<BuiltInKeyword, string>();
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Ananas, ANANAS_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Heuschrecke, HEUSCHRECKE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Himbeere, HIMBEERE_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Leguan, LEGUAN_64);
BUILT_IN_KEYWORD_BYTES.set(
  BuiltInKeyword.Stachelschwein,
  STACHELSCHWEIN_64
);
