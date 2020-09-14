#! /usr/bin/env node
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

const { program } = require("commander");
const Porcupine = require("@picovoice/porcupine-node");
const { getPlatform } = require("@picovoice/porcupine-node/platforms");

const {
  BUILTIN_KEYWORDS_STRINGS,
  BUILTIN_KEYWORDS_STRING_TO_ENUM,
  getBuiltinKeywordPath,
} = require("@picovoice/porcupine-node/builtin_keywords");

const PLATFORM_RECORDER_MAP = new Map();
PLATFORM_RECORDER_MAP.set("linux", "arecord");
PLATFORM_RECORDER_MAP.set("mac", "sox");
PLATFORM_RECORDER_MAP.set("raspberry-pi", "arecord");
PLATFORM_RECORDER_MAP.set("windows", "sox");

const recorder = require("node-record-lpcm16");

program
  .option(
    "-k, --keyword_file_paths <string>",
    "absolute path(s) to porcupine keyword files (.ppn)"
  )
  .option(
    "-b, --keywords <string>",
    `built in keyword(s) (${Array.from(BUILTIN_KEYWORDS_STRINGS)})`
  )
  .option(
    "-l, --library_file_path <string>",
    "absolute path to porcupine dynamic library"
  )
  .option("-m, --model_file_path <string>", "absolute path to porcupine model")
  .option(
    "-s, --sensitivity <number>",
    "sensitivity value between 0 and 1",
    parseFloat,
    0.5
  );

if (process.argv.length < 3) {
  program.help();
}
program.parse(process.argv);

function chunkArray(array, size) {
  return Array.from({ length: Math.ceil(array.length / size) }, (v, index) =>
    array.slice(index * size, index * size + size)
  );
}

function micDemo() {
  let keywordPaths = program["keyword_file_paths"];
  let keywords = program["keywords"];
  let libraryFilePath = program["library_file_path"];
  let modelFilePath = program["model_file_path"];
  let sensitivity = program["sensitivity"];

  let keywordPathsDefined = keywordPaths !== undefined;
  let builtinKeywordsDefined = keywords !== undefined;

  if (
    (keywordPathsDefined && builtinKeywordsDefined) ||
    (!keywordPathsDefined && !builtinKeywordsDefined)
  ) {
    console.error(
      "One of --keyword_file_paths or --keywords is required: Specify a comma-separated list of built-in --keywords (e.g. 'GRASSHOPPER'), or --keyword_file_paths to .ppn files"
    );
    return;
  }

  if (builtinKeywordsDefined) {
    keywordPaths = [];
    for (let builtinKeyword of keywords.split(",")) {
      let keywordString = builtinKeyword.trim().toLowerCase();
      if (BUILTIN_KEYWORDS_STRINGS.has(keywordString)) {
        keywordPaths.push(
          getBuiltinKeywordPath(
            BUILTIN_KEYWORDS_STRING_TO_ENUM.get(keywordString)
          )
        );
      } else {
        console.error(
          `Keyword argument ${builtinKeyword} is not in the list of built-in keywords`
        );
        return;
      }
    }
  }

  if (!Array.isArray(keywordPaths)) {
    keywordPaths = keywordPaths.split(",");
  }
  let keywordNames = [];

  // get the 'friendly' name of the keyword instead of showing index '0','1','2', etc.
  for (let keywordPath of keywordPaths) {
    if (keywordPathsDefined && BUILTIN_KEYWORDS_STRINGS.has(keywordPath)) {
      console.warn(
        `--keyword_path argument '${keywordPath}' matches a built-in keyword. Did you mean to use --keywords ?`
      );
    }
    let keywordName = keywordPath
      .split(/[\\|\/]/)
      .pop()
      .split("_")[0];
    keywordNames.push(keywordName);
  }

  if (isNaN(sensitivity) || sensitivity < 0 || sensitivity > 1) {
    console.error("--sensitivity must be a number in the range [0,1]");
    return;
  }

  // apply the same sensitivity value to all wake words to make running the demo easier
  let sensitivities = [];
  for (let i = 0; i < keywordPaths.length; i++) {
    sensitivities.push(sensitivity);
  }

  let handle = new Porcupine(
    keywordPaths,
    sensitivities,
    modelFilePath,
    libraryFilePath
  );

  let platform;
  try {
    platform = getPlatform();
  } catch (error) {
    console.error();
    ("The NodeJS binding does not support this platform. Supported platforms include macOS (x86_64), Windows (x86_64), Linux (x86_64), and Raspberry Pi (1-4)");
    console.error(error);
  }

  let recorderType = PLATFORM_RECORDER_MAP.get(platform);
  console.log(
    `Platform: '${platform}'; attempting to use '${recorderType}' to access microphone ...`
  );

  const frameLength = handle.frameLength;
  const sampleRate = handle.sampleRate;

  const recording = recorder.record({
    sampleRate: sampleRate,
    channels: 1,
    audioType: "raw",
    recorder: recorderType,
  });

  var frameAccumulator = [];

  recording.stream().on("data", (data) => {
    // Two bytes per Int16 from the data buffer
    let newFrames16 = new Array(data.length / 2);
    for (let i = 0; i < data.length; i += 2) {
      newFrames16[i / 2] = data.readInt16LE(i);
    }

    // Split the incoming PCM integer data into arrays of size Porcupine.frameLength. If there's insufficient frames, or a remainder,
    // store it in 'frameAccumulator' for the next iteration, so that we don't miss any audio data
    frameAccumulator = frameAccumulator.concat(newFrames16);
    let frames = chunkArray(frameAccumulator, frameLength);

    if (frames[frames.length - 1].length !== frameLength) {
      // store remainder from divisions of frameLength
      frameAccumulator = frames.pop();
    } else {
      frameAccumulator = [];
    }

    for (let frame of frames) {
      let index = handle.process(frame);
      if (index !== -1) {
        console.log(`Detected '${keywordNames[index]}'`);
      }
    }
  });

  console.log(`Listening for wake word(s): ${keywordNames}`);
  process.stdin.resume();
  console.warn("Press ctrl+c to exit.");
}

micDemo();
