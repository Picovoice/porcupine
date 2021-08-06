/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

mod porcupine;
mod util;

pub use porcupine::*;
pub use util::*;

pub const KEYWORDS: [&str; 15] = [
    "alexa",
    "americano",
    "blueberry",
    "bumblebee",
    "computer",
    "grapefruit",
    "grasshopper",
    "hey barista",
    "hey google",
    "hey siri",
    "jarvis",
    "ok google",
    "picovoice",
    "porcupine",
    "terminator",
];
