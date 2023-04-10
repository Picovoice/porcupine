import { BuiltInKeyword, Porcupine, PorcupineKeyword, PorcupineWorker } from "../";

import { PvModel } from '@picovoice/web-utils';

const ACCESS_KEY = Cypress.env('ACCESS_KEY');
const NUM_TEST_ITERATIONS = Number(Cypress.env('NUM_TEST_ITERATIONS'));
const INIT_PERFORMANCE_THRESHOLD_SEC = Number(Cypress.env('INIT_PERFORMANCE_THRESHOLD_SEC'));
const PROC_PERFORMANCE_THRESHOLD_SEC = Number(Cypress.env('PROC_PERFORMANCE_THRESHOLD_SEC'));

async function testPerformance(
  instance: typeof Porcupine | typeof PorcupineWorker,
  inputPcm: Int16Array,
  params: {
    keyword?: BuiltInKeyword | PorcupineKeyword | (BuiltInKeyword | PorcupineKeyword)[]
    model?: PvModel,
  } = {}
) {
  const {
    keyword = BuiltInKeyword.Porcupine,
    model = { publicPath: '/test/porcupine_params.pv', forceWrite: true }
  } = params;

  const initPerfResults: number[] = [];
  const procPerfResults: number[] = [];

  for (let j = 0; j < NUM_TEST_ITERATIONS; j++) {
    let detected = false;

    let start = Date.now();
    const porcupine = await instance.create(
      ACCESS_KEY,
      keyword,
      () => {
        detected = true;
      },
      model
    );

    let end = Date.now();
    initPerfResults.push((end - start) / 1000);

    const waitUntil = (): Promise<void> => new Promise(resolve => {
      setInterval(() => {
        if (detected) {
          resolve();
        }
      }, 100);
    });

    start = Date.now();
    for (let i = 0; i < (inputPcm.length - porcupine.frameLength + 1); i += porcupine.frameLength) {
      await porcupine.process(inputPcm.slice(i, i + porcupine.frameLength));
    }
    await waitUntil();
    end = Date.now();
    procPerfResults.push((end - start) / 1000);

    if (porcupine instanceof PorcupineWorker) {
      porcupine.terminate();
    } else {
      await porcupine.release();
    }
  }

  const initAvgPerf = initPerfResults.reduce((a, b) => a + b) / NUM_TEST_ITERATIONS;
  const procAvgPerf = procPerfResults.reduce((a, b) => a + b) / NUM_TEST_ITERATIONS;

  // eslint-disable-next-line no-console
  console.log(`Average init performance: ${initAvgPerf} seconds`);
  // eslint-disable-next-line no-console
  console.log(`Average proc performance: ${procAvgPerf} seconds`);

  expect(initAvgPerf).to.be.lessThan(INIT_PERFORMANCE_THRESHOLD_SEC);
  expect(procAvgPerf).to.be.lessThan(PROC_PERFORMANCE_THRESHOLD_SEC);
}

describe('Porcupine binding performance test', () => {
  for (const instance of [Porcupine, PorcupineWorker]) {
    const instanceString = (instance === PorcupineWorker) ? 'worker' : 'main';

    it(`should be lower than performance threshold (${instanceString})`, () => {
      cy.getFramesFromFile('audio_samples/porcupine.wav').then( async inputPcm => {
        await testPerformance(
          instance,
          inputPcm
        );
      });
    });
  }
});
