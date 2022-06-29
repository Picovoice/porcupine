import * as path from "path";
import * as fs from "fs";

import {WaveFile} from "wavefile";
import {checkWaveFile, getInt16Frames} from "../src";
import { getPlatform, getSystemLibraryPath } from "../src/platforms";

function appendLanguage(
    s: string,
    language: string): string {
    if (language === "en") {
        return s;
    } else {
        return s + "_" + language;
    }
}

// def _append_language(s, language):
// if language == 'en':
// return s
// return "%s_%s" % (s, language)

// export function rhinoProcessWaveFile(
//     engineInstance: Rhino,
//     relativeWaveFilePath: string,
//     ignoreIsFinalized: boolean = false
// ): RhinoInference {
//     const waveFilePath = path.join(__dirname, relativeWaveFilePath);
//     const waveBuffer = fs.readFileSync(waveFilePath);
//     const waveAudioFile = new WaveFile(waveBuffer);
//
//     if (!checkWaveFile(waveAudioFile, engineInstance.sampleRate)) {
//         fail("Audio file did not meet requirements. Wave file must be 16KHz, 16-bit, linear PCM (mono).");
//     }
//
//     const frames = getInt16Frames(waveAudioFile, engineInstance.frameLength);
//
//     let isFinalized = false;
//     for (let i = 0; i < frames.length; i++) {
//         const frame = frames[i];
//         isFinalized = engineInstance.process(frame) || ignoreIsFinalized;
//
//         if (isFinalized) {
//             return engineInstance.getInference();
//         }
//     }
//
//     fail("failed to process wave file");
// }

// def read_wav_file(file_name, sample_rate):
// wav_file = wave.open(file_name, mode="rb")
// channels = wav_file.getnchannels()
// num_frames = wav_file.getnframes()
//
// if wav_file.getframerate() != sample_rate:
// raise ValueError(
//     "Audio file should have a sample rate of %d, got %d" % (sample_rate, wav_file.getframerate()))
//
// samples = wav_file.readframes(num_frames)
// wav_file.close()
//
// frames = struct.unpack('h' * num_frames * channels, samples)
//
// if channels == 2:
// print("Picovoice processes single-channel audio but stereo file is provided. Processing left channel only.")
//
// return frames[::channels]

export function getModelPathByLanguage(
    relativeModelPath: string,
    language: string): string {
    return path.join(
        __dirname,
        relativeModelPath,
        `${appendLanguage('lib/common/porcupine_params', language)}.pv`);
}

// def get_model_path_by_language(relative, language):
// model_path_subdir = _append_language('lib/common/porcupine_params', language)
// return os.path.join(os.path.dirname(__file__), relative, '%s.pv' % model_path_subdir)


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

// def get_keyword_paths_by_language(relative, language, keywords):
// keyword_files_root = _append_language('resources/keyword_files', language)
// keyword_files_dir = os.path.join(
//     os.path.dirname(__file__),
//     relative,
//     keyword_files_root,
//     pv_keyword_files_subdir())
//
// res = list()
// keyword_paths = {
//     keyword_file.rsplit('_')[0]: os.path.join(keyword_files_dir, keyword_file)
// for keyword_file in os.listdir(keyword_files_dir)
//     }
//
// for keyword in keywords:
// res.append(keyword_paths[keyword])
//
// return res
//
//
// def get_audio_file_by_language(relative, language, audio_file_name=None):
// if audio_file_name is None:
//     audio_file_name = "%s.wav" % _append_language('multiple_keywords', language)
// return os.path.join(os.path.dirname(__file__), relative, 'resources/audio_samples', audio_file_name)

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