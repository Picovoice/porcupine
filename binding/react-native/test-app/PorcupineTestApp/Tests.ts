import {Platform} from 'react-native';
import fs from 'react-native-fs';
import {decode as atob} from 'base-64';

import {Porcupine} from '@picovoice/porcupine-react-native';

const testData = require('./test_data.json');
const platform = Platform.OS;

const accessKey: string = '{TESTING_ACCESS_KEY_HERE}';

export type Result = {
  testName: string;
  success: boolean;
  errorString?: string;
};

function getPath(filePath) {
  if (platform === "ios") {
    return `Assets.bundle/${filePath}`;
  }
  return filePath;
}

async function getBinaryFile(audioFilePath) {
  let fileBase64;
  if (platform === "ios") {
    fileBase64 = await fs.readFile(`${fs.MainBundlePath}/${audioFilePath}`, 'base64');
  } else {
    fileBase64 = await fs.readFileAssets(audioFilePath, 'base64');
  }
  const fileBinary = atob(fileBase64);

  const bytes = new Uint8Array(fileBinary.length);
  for (let i = 0; i < fileBinary.length; i++) {
    bytes[i] = fileBinary.charCodeAt(i);
  }
  return bytes;
}

async function getPcmFromFile(audioFilePath, expectedSampleRate) {
  const headerSampleRateOffset = 24;
  const headerOffset = 44;

  const fileBytes = await getBinaryFile(audioFilePath);
  const dataView = new DataView(fileBytes.buffer);

  const fileSampleRate = dataView.getInt32(headerSampleRateOffset, true);
  if (fileSampleRate !== expectedSampleRate) {
    throw new Error(
      `Specified sample rate did not match test file: '${fileSampleRate}' != '${expectedSampleRate}'`,
    );
  }

  const pcm = [];
  for (let i = headerOffset; i < fileBytes.length; i += 2) {
    pcm.push(dataView.getInt16(i, true));
  }

  return pcm;
}

async function getDetections(porcupine, audioFilePath) {
  const detections = [];

  const pcm = await getPcmFromFile(audioFilePath, porcupine.sampleRate);
  const frameLength = porcupine.frameLength;
  for (let i = 0; i < pcm.length - frameLength; i += frameLength) {
    const detection = await porcupine.process(pcm.slice(i, i + frameLength));
    if (detection !== -1) {
      detections.push(detection);
    }
  }

  return detections;
}

function arraysEqual(a, b) {
  if (a === b) {
    return true;
  }
  if (a == null || b == null) {
    return false;
  }
  if (a.length !== b.length) {
    return false;
  }

  for (var i = 0; i < a.length; ++i) {
    if (a[i] !== b[i]) {
      return false;
    }
  }

  return true;
}

async function runTestcase(
  language,
  keywords,
  audioFilePath,
  groundTruth,
): Result {
  const result = {testName: '', success: false};
  let porcupine = null;
  try {
    const keywordPaths = keywords.map(
      keyword => getPath(`keyword_files/${language}/${keyword}_${platform}.ppn`),
    );
    const modelPath =
      language === 'en'
        ? getPath('model_files/porcupine_params.pv')
        : getPath(`model_files/porcupine_params_${language}.pv`);
    porcupine = await Porcupine.fromKeywordPaths(
      accessKey,
      keywordPaths,
      modelPath,
    );
    const detections = await getDetections(porcupine, audioFilePath);
    if (arraysEqual(detections, groundTruth)) {
      result.success = true;
    } else {
      result.success = false;
      result.errorString = `Detections '${JSON.stringify(
        detections,
      )}' does not equal ground truth '${JSON.stringify(groundTruth)}'`;
    }
    await porcupine.delete();
  } catch (error) {
    result.success = false;
    result.errorString = `${error}`;
  }
  return result;
}

async function singleKeywordTest(testcases): Result[] {
  const results = [];
  for (const testcase of testcases) {
    const audioFilePath = getPath(`audio_samples/${testcase.wakeword.replaceAll(
      ' ',
      '_',
    )}.wav`);
    const result = await runTestcase(
      testcase.language,
      [testcase.wakeword],
      audioFilePath,
      [0],
    );
    result.testName = `Single keyword test for ${testcase.language} ${testcase.wakeword}`;
    results.push(result);
  }
  return results;
}

async function multipleKeywordTest(testcases): Result[] {
  const results = [];
  for (const testcase of testcases) {
    const audioFilePath =
      testcase.language === 'en'
        ? getPath('audio_samples/multiple_keywords.wav')
        : getPath(`audio_samples/multiple_keywords_${testcase.language}.wav`);
    const result = await runTestcase(
      testcase.language,
      testcase.wakewords,
      audioFilePath,
      testcase.groundTruth,
    );
    result.testName = `Multiple keyword test for ${testcase.language}`;
    results.push(result);
  }
  return results;
}

export async function runPorcupineTests(): Result[] {
  const singleKeywordResults = await singleKeywordTest(
    testData.tests.singleKeyword,
  );
  const multipleKeywordResults = await multipleKeywordTest(
    testData.tests.multipleKeyword,
  );
  return [...singleKeywordResults, ...multipleKeywordResults];
}
