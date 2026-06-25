import {
  BuiltInKeyword,
  Porcupine,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineWorker,
} from '../';

import { PvModel } from '@picovoice/web-utils';

const ACCESS_KEY: string = Cypress.env('ACCESS_KEY');

const runInitTest = async (
  instance: typeof Porcupine | typeof PorcupineWorker,
  params: {
    accessKey?: string;
    keyword?:
      | BuiltInKeyword
      | PorcupineKeyword
      | (BuiltInKeyword | PorcupineKeyword)[];
    model?: PvModel;
    device?: string,
    expectFailure?: boolean;
  } = {}
) => {
  const {
    accessKey = ACCESS_KEY,
    keyword = BuiltInKeyword.Porcupine,
    model = { publicPath: '/test/porcupine_params.pv', forceWrite: true },
    device = "cpu:1",
    expectFailure = false,
  } = params;

  let isFailed = false;

  try {
    const porcupine = await instance.create(
      accessKey,
      keyword,
      () => {},
      model,
      { device }
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

describe("Porcupine Train", function () {
  it(`should be able to train model`, () => {
    const writePath = "custom_phrase_wasm.rhn";

    const model: PorcupineModel = {
      publicPath: "/test/porcupine_params_fr.pv",
      forceWrite: true,
    };

    cy.wrap(null).then(async () => {
      const porcupineKeyword = await Porcupine.trainWakeWordFromPhrase(
        ACCESS_KEY,
        writePath,
        "fr",
        "vérifier"
      );

      await runInitTest(Porcupine, {
        keyword: porcupineKeyword,
        model,
      });
    });
  });

  it(`should be able to handle invalid phrase`, () => {
    const writePath = "custom_phrase_wasm.rhn";

    cy.wrap(null).then(async () => {
      let failed = false;
      try {
        const porcupineKeyword = await Porcupine.trainWakeWordFromPhrase(
          ACCESS_KEY,
          writePath,
          "fr",
          "ㅁㄴㅇㄹ"
        );
        expect(porcupineKeyword).to.be.null;
      } catch (e) {
        expect(e).to.not.be.null;
        failed = true;
      }
      expect(failed).to.be.true;
    });
  });
});
