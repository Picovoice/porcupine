//
// Copyright 2020 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

const SUCCESS = 0;
const OUT_OF_MEMORY = 1;
const IO_ERROR = 2;
const INVALID_ARGUMENT = 3;
const STOP_ITERATION = 4;
const KEY_ERROR = 5;
const INVALID_STATE = 6;

module.exports = {
  SUCCESS: SUCCESS,
  OUT_OF_MEMORY: OUT_OF_MEMORY,
  IO_ERROR: IO_ERROR,
  INVALID_ARGUMENT: INVALID_ARGUMENT,
  STOP_ITERATION: STOP_ITERATION,
  KEY_ERROR: KEY_ERROR,
  INVALID_STATE: INVALID_STATE,
};
