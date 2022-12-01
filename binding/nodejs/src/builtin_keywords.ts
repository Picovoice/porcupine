//
// Copyright 2020-2022 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

import * as path from "path";

import { getPlatform } from "./platforms";

export enum BuiltinKeyword {
  ALEXA = "alexa",
  AMERICANO = "americano",
  BLUEBERRY = "blueberry",
  BUMBLEBEE = "bumblebee",
  COMPUTER = "computer",
  GRAPEFRUIT = "grapefruit",
  GRASSHOPPER = "grasshopper",
  HEY_GOOGLE = "hey google",
  HEY_SIRI = "hey siri",
  JARVIS = "jarvis",
  OK_GOOGLE = "ok google",
  PICOVOICE = "picovoice",
  PORCUPINE = "porcupine",
  TERMINATOR = "terminator",
}

export function getBuiltinKeywordPath(builtinKeyword: BuiltinKeyword): string {
  const platform = getPlatform();
  return path.resolve(
    __dirname,
    `../resources/keyword_files/${platform}/${builtinKeyword}_${platform}.ppn`
  );
}
