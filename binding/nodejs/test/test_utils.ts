import * as path from "path";
import { getPlatform } from "../src/platforms";

const ROOT_DIR = path.join(__dirname, "../../..");
const TEST_DATA_JSON = require(path.join(ROOT_DIR, 'resources/.test/test_data.json'));

function appendLanguage(
  s: string,
  language: string): string {
  if (language === "en") {
    return s;
  }
  return s + "_" + language;
}

export function getModelPathByLanguage(language: string): string {
  return path.join(
    ROOT_DIR,
    `${appendLanguage('lib/common/porcupine_params', language)}.pv`);
}

export function getKeywordPathsByLanguage(
  language: string,
  keyword: string): string {
  return path.join(
    ROOT_DIR,
    appendLanguage('resources/keyword_files', language),
    getPlatform(),
    `${keyword}_${getPlatform()}.ppn`);
}

export function getAudioFileByLanguage(
  language: string,
  audioFile: string | null = null): string {
  if (audioFile === null) {
    // eslint-disable-next-line
    audioFile = `${appendLanguage('multiple_keywords', language)}.wav`;
  }

  return path.join(
    ROOT_DIR,
    'resources/audio_samples',
    audioFile);
}

export function getSingleKeywordParameters(): [string, string, string][] {
  let singleKeywordJson = TEST_DATA_JSON.tests.singleKeyword;
  return singleKeywordJson.map(
    (x: any) => [x.language, x.wakeword, x.wakeword.replace(" ", "_") + ".wav"]
  );
}

export function getMultipleKeywordParameters(): [string, string[], number[]][] {
  let multipleKeywordJson = TEST_DATA_JSON.tests.multipleKeyword;
  return multipleKeywordJson.map(
    (x: any) => [x.language, x.wakewords, x.groundTruth]
  );
}
