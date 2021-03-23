/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import {
  EMPAREDADO_64,
  LEOPARDO_64,
  MANZANA_64,
  MURCIELAGO_64,
} from './dist/built_in_keywords_64';

export enum BuiltInKeyword {
  Emparedado = 'Emparedado',
  Leopardo = 'Leopardo',
  Manzana = 'Manzana',
  Murcielago = 'Murcielago',
}

export const BUILT_IN_KEYWORD_BYTES = new Map<BuiltInKeyword, string>();
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Emparedado, EMPAREDADO_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Leopardo, LEOPARDO_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Manzana, MANZANA_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Murcielago, MURCIELAGO_64);
