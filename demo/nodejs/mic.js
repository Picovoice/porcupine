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
const {
  Porcupine,
  BuiltinKeyword,
  getBuiltinKeywordPath,
} = require("@picovoice/porcupine-node");
const PvRecorder = require("@picovoice/pvrecorder-node");

program
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
  ).option(
      "-i, --audio_device_index <number>",
      "index of audio device to use to record audio",
      Number,
      -1
  ).option(
      "-d, --show_audio_devices",
      "show the list of available devices"
  );

if (process.argv.length < 3) {
  program.help();
}
program.parse(process.argv);

let isInterrupted = false;

async function micDemo() {
  let accessKey = program["access_key"]  
  let keywordPaths = program["keyword_file_paths"];
  let keywords = program["keywords"];
  let libraryFilePath = program["library_file_path"];
  let modelFilePath = program["model_file_path"];
  let sensitivity = program["sensitivity"];
  let audioDeviceIndex = program["audio_device_index"];
  let showAudioDevices = program["show_audio_devices"];

  let keywordPathsDefined = keywordPaths !== undefined;
  let builtinKeywordsDefined = keywords !== undefined;
  let showAudioDevicesDefined = showAudioDevices !== undefined;

  if (showAudioDevicesDefined) {
      const devices = PvRecorder.getAudioDevices();
      for (let i = 0; i < devices.length; i++) {
          console.log(`index: ${i}, device name: ${devices[i]}`);
      }
      process.exit();
  }

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

  let handle = new Porcupine(
    accessKey,  
    keywordPaths,
    sensitivities,
    modelFilePath,
    libraryFilePath
  );

  const frameLength = handle.frameLength;

  const recorder = new PvRecorder(audioDeviceIndex, frameLength);
  recorder.start();

  console.log(`Using device: ${recorder.getSelectedDevice()}...`);

  console.log(`Listening for wake word(s): ${keywordNames}`);
  console.log("Press ctrl+c to exit.")

  while (!isInterrupted) {
      const pcm = await recorder.read();
      let index = handle.process(pcm);
      if (index !== -1) {
        console.log(`Detected '${keywordNames[index]}'`);
      }
  }

  console.log("Stopping...");
  recorder.release();
}

// setup interrupt
process.on("SIGINT", function () {
    isInterrupted = true;
});

(async function () {
    try {
        await micDemo();
    } catch (e) {
        console.error(e.toString());
    }
})();
