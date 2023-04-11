import { BuiltInKeyword, Porcupine, PorcupineKeyword, PorcupineWorker } from "../";
import testData from "./test_data.json";

// @ts-ignore
import porcupineParams from "./porcupine_params";
import { PvModel } from '@picovoice/web-utils';

const ACCESS_KEY: string = Cypress.env("ACCESS_KEY");


function delay(time: number) {
  return new Promise(resolve => setTimeout(resolve, time));
}

const runInitTest = async (
  instance: typeof Porcupine | typeof PorcupineWorker,
  params: {
    accessKey?: string,
    keyword?: (BuiltInKeyword | PorcupineKeyword | (BuiltInKeyword | PorcupineKeyword)[]),
    model?: PvModel,
    expectFailure?: boolean,
  } = {}
) => {
  const {
    accessKey = ACCESS_KEY,
    keyword = BuiltInKeyword.Porcupine,
    model = { publicPath: '/test/porcupine_params.pv', forceWrite: true },
    expectFailure = false,
  } = params;

  let isFailed = false;

  try {
    const porcupine = await instance.create(
      accessKey,
      keyword,
      () => {},
      model
    );
    expect(porcupine.sampleRate).to.be.eq(16000);
    expect(typeof porcupine.version).to.eq('string');
    expect(porcupine.version.length).to.be.greaterThan(0);

    if (porcupine instanceof PorcupineWorker) {
      porcupine.terminate();
    } else {
      await porcupine.release();
    }
  } catch (e) {
    if (expectFailure) {
      isFailed = true;
    } else {
      expect(e).to.be.undefined;
    }
  }

  if (expectFailure) {
    expect(isFailed).to.be.true;
  }
};

const runProcTest = async (
  instance: typeof Porcupine | typeof PorcupineWorker,
  inputPcm: Int16Array,
  expectedDetections: number[],
  params: {
    accessKey?: string,
    keyword?: BuiltInKeyword | PorcupineKeyword | (BuiltInKeyword | PorcupineKeyword)[]
    model?: PvModel,
  } = {}
) => {
  const {
    accessKey = ACCESS_KEY,
    keyword = BuiltInKeyword.Porcupine,
    model = { publicPath: '/test/porcupine_params.pv', forceWrite: true },
  } = params;

  const detections: number[] = [];

  const runProcess = () => new Promise<void>(async (resolve, reject) => {
    const porcupine = await instance.create(
      accessKey,
      keyword,
      async porcupineDetection => {
        detections.push(porcupineDetection.index);
        if (detections.length === expectedDetections.length) {
          resolve();
        }
      },
      model,
      {
        processErrorCallback: (error: string) => {
          reject(error);
        }
      }
    );

    for (let i = 0; i < (inputPcm.length - porcupine.frameLength + 1); i += porcupine.frameLength) {
      await porcupine.process(inputPcm.slice(i, i + porcupine.frameLength));
    }

    await delay(1000);

    if (porcupine instanceof PorcupineWorker) {
      porcupine.terminate();
    } else {
      await porcupine.release();
    }
  });

  try {
    await runProcess();
    expect(detections).to.deep.eq(expectedDetections);
  } catch (e) {
    expect(e).to.be.undefined;
  }
};

describe("Porcupine Binding", function () {
  for (const instance of [Porcupine, PorcupineWorker]) {
    const instanceString = (instance === PorcupineWorker) ? 'worker' : 'main';

    it(`should be able to init with public path (${instanceString})`, () => {
      cy.wrap(null).then(async () => {
        await runInitTest(instance);
      });
    });

    it(`should be able to init with base64 (${instanceString})`, () => {
      cy.wrap(null).then(async () => {
        await runInitTest(instance, {
          model: { base64: porcupineParams, forceWrite: true }
        });
      });
    });

    it(`should be able to handle UTF-8 public path (${instanceString})`, () => {
      cy.wrap(null).then(async () => {
        await runInitTest(instance, {
          model: { publicPath: '/test/porcupine_params.pv', forceWrite: true, customWritePath: '테스트' }
        });
      });
    });

    it(`should be able to handle invalid public path (${instanceString})`, () => {
      cy.wrap(null).then(async () => {
        await runInitTest(instance, {
          model: { publicPath: 'invalid', forceWrite: true },
          expectFailure: true
        });
      });
    });

    it(`should be able to handle invalid base64 (${instanceString})`, () => {
      cy.wrap(null).then(async () => {
        await runInitTest(instance, {
          model: { base64: 'invalid', forceWrite: true },
          expectFailure: true
        });
      });
    });

    it(`should be able to handle invalid access key (${instanceString})`, () => {
      cy.wrap(null).then(async () => {
        await runInitTest(instance, {
          accessKey: 'invalid',
          expectFailure: true
        });
      });
    });

    it(`should be able to handle invalid sensitivity(${instanceString})`, () => {
      cy.wrap(null).then(async () => {
        await runInitTest(instance, {
          keyword: { publicPath: '/test/keywords/porcupine.ppn', forceWrite: true, sensitivity: -1, label: 'porcupine' },
          expectFailure: true
        });
      });
    });

    for (const testParam of testData.tests.singleKeyword) {
      it(`should be able to process single keyword (${testParam.language}) (${instanceString})`, () => {
        try {
          cy.getFramesFromFile(`audio_samples/${testParam.wakeword.replace(" ", "_")}.wav`).then( async pcm => {
            const suffix = (testParam.language === 'en') ? '' : `_${testParam.language}`;
            await runProcTest(
              instance,
              pcm,
              [0],
              {
                keyword: {
                  publicPath: `/test/keywords/${encodeURIComponent(testParam.wakeword)}_wasm.ppn`,
                  forceWrite: true,
                  label: testParam.wakeword
                },
                model: { publicPath: `/test/porcupine_params${suffix}.pv`, forceWrite: true }
              });
          });
        } catch (e) {
          expect(e).to.be.undefined;
        }
      });
    }

    for (const testParam of testData.tests.multipleKeyword) {
      it(`should be able to process multiple keyword (${testParam.language}) (${instanceString})`, () => {
        try {
          const suffix = (testParam.language === 'en') ? '' : `_${testParam.language}`;
          cy.getFramesFromFile(`audio_samples/multiple_keywords${suffix}.wav`).then( async pcm => {
            const keywords: PorcupineKeyword[] = testParam.wakewords.map(wakeword =>
              ({
                publicPath: `/test/keywords/${encodeURIComponent(wakeword.replace("ż", "z"))}_wasm.ppn`,
                forceWrite: true,
                label: wakeword
              })
            );

            await runProcTest(
              instance,
              pcm,
              testParam.groundTruth,
              {
                keyword: keywords,
                model: { publicPath: `/test/porcupine_params${suffix}.pv`, forceWrite: true }
              });
          });
        } catch (e) {
          expect(e).to.be.undefined;
        }
      });
    }
  }
});
