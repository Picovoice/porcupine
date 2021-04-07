import { Porcupine } from '../dist/esm/index';
import { getInt16Frames, checkWaveFile } from './wave_util';
import fs from 'fs';
import { WaveFile } from 'wavefile';

const WAV_PATH_PORCUPINE = '../../../resources/audio_samples/porcupine.wav';
const WAV_PATH_MULTIPLE_KEYWORDS =
  '../../../resources/audio_samples/multiple_keywords.wav';

function porcupineDetectionCounts(engineInstance, waveFilePath) {
  const waveBuffer = fs.readFileSync(waveFilePath);
  const waveAudioFile = new WaveFile(waveBuffer);

  if (!checkWaveFile(waveAudioFile, engineInstance.sampleRate)) {
    console.error(
      'Audio file did not meet requirements. Wave file must be 16KHz, 16-bit, linear PCM (mono).'
    );
    return null;
  }

  const frames = getInt16Frames(waveAudioFile, engineInstance.frameLength);

  // build a map of keyword indices to detection counts
  let detections = new Map();

  for (let i = 0; i < frames.length; i++) {
    const frame = frames[i];
    const keywordIndex = engineInstance.process(frame);

    if (keywordIndex !== -1) {
      let currentCount = detections.get(keywordIndex);

      if (currentCount === undefined) {
        detections.set(keywordIndex, 1);
      } else {
        detections.set(keywordIndex, currentCount + 1);
      }
    }
  }
  return detections;
}

describe('successful keyword detections', () => {
  test('single keyword single detection', async () => {
    let handle = await Porcupine.create('Porcupine');

    let counts = porcupineDetectionCounts(handle, WAV_PATH_PORCUPINE);
    let porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(1);
  });

  test("builtin keyword 'Grasshopper'", async () => {
    let porcupineEngine = await Porcupine.create({ builtin: 'Grasshopper' });
    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    let grasshopperCount = counts.get(0);
    expect(grasshopperCount).toEqual(1);
  });

  test('invalid builtin keyword type', async () => {
    await expect(Porcupine.create(123213)).rejects.toThrow(/Invalid/);
  });

  test('single keyword multiple detection', async () => {
    let porcupineEngine = await Porcupine.create('Porcupine');

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    let porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(2);
  });

  test('multiple keyword multiple detection', async () => {
    let porcupineEngine = await Porcupine.create(['Americano', 'Bumblebee']);

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    let americanoCount = counts.get(0);
    let bumblebeeCount = counts.get(1);
    expect(americanoCount).toEqual(1);
    expect(bumblebeeCount).toEqual(1);
  });
});

describe('basic parameter validation', () => {
  test('sensitivity is not in range', async () => {
    await expect(
      Porcupine.create({
        builtin: 'Picovoice',
        sensitivity: 4.2,
      })
    ).rejects.toThrow(Error);
  });

  test('sensitivity is not a number', async () => {
    await expect(
      Porcupine.create({
        builtin: 'Picovoice',
        sensitivity: 'steamed hams',
      })
    ).rejects.toThrow(Error);
  });
});
