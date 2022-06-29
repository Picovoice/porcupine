import * as path from "path";
import {getPlatform} from "../src/platforms";

function appendLanguage(
    s: string,
    language: string): string {
    if (language === "en") {
        return s;
    } else {
        return s + "_" + language;
    }
}

export function getModelPathByLanguage(
    relativeModelPath: string,
    language: string): string {
    return path.join(
        __dirname,
        relativeModelPath,
        `${appendLanguage('lib/common/porcupine_params', language)}.pv`);
}

export function getKeywordPathsByLanguage(
    relativeKeywordPath: string,
    language: string,
    keyword: string): string {
    return path.join(
        __dirname,
        relativeKeywordPath,
        appendLanguage('resources/keyword_files', language),
        getPlatform(),
        `${keyword}_${getPlatform()}.ppn`);
}

export function getAudioFileByLanguage(
    relativeAudioFilePath: string,
    language: string,
    audioFile: string | null = null): string {
    if (audioFile === null) {
        audioFile = `${appendLanguage('multiple_keywords', language)}.wav`;
    }

    return path.join(
        __dirname,
        relativeAudioFilePath,
        'resources/audio_samples',
        audioFile);
}