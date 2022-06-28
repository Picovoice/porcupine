/*
  Copyright 2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import {
  BUSHI_64,
  NINJA_64,
  RINGO_64,
} from './dist/built_in_keywords_64';

export enum BuiltInKeyword {
  Bushi = 'Bushi',
  Ninja = 'Ninja',
  Ringo = 'Ringo',
}

export const BUILT_IN_KEYWORD_BYTES = new Map<BuiltInKeyword, string>();
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Bushi, BUSHI_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Ninja, NINJA_64);
BUILT_IN_KEYWORD_BYTES.set(BuiltInKeyword.Ringo, RINGO_64);
