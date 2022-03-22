#! /usr/bin/env node
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

const { program } = require("commander");
const fs = require("fs");

const WaveFile = require("wavefile").WaveFile;
const {
  Porcupine,
  BuiltinKeyword,
  getBuiltinKeywordPath,
  getInt16Frames,
  checkWaveFile,
} = require("@picovoice/porcupine-node");

program
  .requiredOption(
    "-i, --input_audio_file_path <string>",
    "input audio wave file in 16-bit 16KHz linear PCM format (mono)"
  )
  .requiredOption(
    "-a, --access_key <string>",
    "AccessKey obtain from the Picovoice Console (https://console.picovoice.ai/)"
  )
  .option(
    "-k, --keyword_file_paths <string>",
    "absolute path(s) to porcupine keyword files (.ppn)"
  )
  .option(
    "-b, --keywords <string>",
    `built in keyword(s) (${Object.keys(BuiltinKeyword)})`
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

function frameIndexToSeconds(frameIndex, engineInstance) {
  return (frameIndex * engineInstance.frameLength) / engineInstance.sampleRate;
}

function fileDemo() {
  let audioPath = program["input_audio_file_path"];
  let accessKey = program["access_key"]
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
      let keywordString = builtinKeyword.trim().toUpperCase();
      if (keywordString in BuiltinKeyword) {
        keywordPaths.push(
          getBuiltinKeywordPath(
            BuiltinKeyword[keywordString]
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
    if (keywordPathsDefined && keywordPath in BuiltinKeyword) {
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

  if (!fs.existsSync(audioPath)) {
    console.error(`--input_audio_file_path file not found: ${audioPath}`);
    return;
  }

  let engineInstance;
  try {
    engineInstance = new Porcupine(
      accessKey,
      keywordPaths,
      sensitivities,
      modelFilePath,
      libraryFilePath
    );
  } catch (error) {
    console.error(`Error initializing Porcupine engine: ${error}`);
    return;
  }

  let waveBuffer = fs.readFileSync(audioPath);
  let inputWaveFile;
  try {
    inputWaveFile = new WaveFile(waveBuffer);
  } catch (error) {
    console.error(`Exception trying to read file as wave format: ${audioPath}`);
    console.error(error);
    return;
  }

  if (!checkWaveFile(inputWaveFile, engineInstance.sampleRate)) {
    console.error(
      "Audio file did not meet requirements. Wave file must be 16KHz, 16-bit, linear PCM (mono)."
    );
  }

  let frames = getInt16Frames(inputWaveFile, engineInstance.frameLength);

  for (let i = 0; i < frames.length; i++) {
    const frame = frames[i];
    const keywordIndex = engineInstance.process(frame);

    if (keywordIndex !== -1) {
      const timestamp = frameIndexToSeconds(i, engineInstance);
      console.log(
        `Detected keyword '${keywordNames[keywordIndex]}' @ ${timestamp}s`
      );
    }
  }

  engineInstance.release();
}

fileDemo();
