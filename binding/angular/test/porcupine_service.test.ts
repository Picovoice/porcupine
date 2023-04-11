import { BuiltInKeyword } from '@picovoice/porcupine-web';

import { PorcupineService } from '../dist/porcupine-angular';

import porcupineParams from './porcupine_params.js';
import testData from './test_data.json';

const ACCESS_KEY = Cypress.env('ACCESS_KEY');

describe('Porcupine Binding', () => {
  it("should be able to init via public path", (done) => {
    let i = 0;
    const expected = [true, false];

    const porcupineService = new PorcupineService();

    cy.wrapFn(
      () =>
        porcupineService.init(
          ACCESS_KEY,
          BuiltInKeyword.Porcupine,
          { publicPath: './porcupine_params.pv', forceWrite: true }
        )
    );

    cy.wrapFn(
      () => porcupineService.release()
    );

    porcupineService.isLoaded$.subscribe(isLoaded => {
      expect(isLoaded).to.eq(expected[i++]);
      if (i == expected.length) {
        done();
      }
    });
  });

  it('should be able to init via base64', (done) => {
    const porcupineService = new PorcupineService();

    cy.wrapFn(
      () => porcupineService.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { base64: porcupineParams, forceWrite: true }
      )
    );

    cy.wrapFn(
      () => porcupineService.release()
    );

    porcupineService.isLoaded$.subscribe(isLoaded => {
      expect(isLoaded).to.be.true;
      done();
    });
  });

  it('should show invalid model path error message', (done) => {
    const porcupineService = new PorcupineService();

    cy.wrapFn(
      () => porcupineService.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { publicPath: "./porcupine_params_failed.pv", forceWrite: true }
      )
    );

    porcupineService.isLoaded$.subscribe(isLoaded => {
      expect(isLoaded).to.be.false;
    });

    porcupineService.error$.subscribe(error => {
      expect(error?.toString()).to.contain("Error response returned while fetching model from './porcupine_params_failed.pv'");
      done();
    });
  });

  it('should show invalid access key error message', (done) => {
    const porcupineService = new PorcupineService();

    cy.wrapFn(
      () => porcupineService.init(
        '',
        BuiltInKeyword.Porcupine,
        { publicPath: "./porcupine_params.pv", forceWrite: true }
      )
    );

    porcupineService.isLoaded$.subscribe(isLoaded => {
      expect(isLoaded).to.be.false;
    });

    porcupineService.error$.subscribe(error => {
      expect(error?.toString()).to.contain("Invalid AccessKey");
      done();
    });
  });

  for (const testInfo of testData.tests.singleKeyword) {
    it(`should be able to process audio (${testInfo.language})`, (done) => {
      const porcupineService = new PorcupineService();

      cy.wrapFn(
        () => porcupineService.init(
          ACCESS_KEY,
          {
            label: testInfo.wakeword,
            publicPath: `./keywords/${testInfo.wakeword}_wasm.ppn`,
            forceWrite: true,
          },
          {
            publicPath: testInfo.language === 'en' ? "./porcupine_params.pv" : `./porcupine_params_${testInfo.language}.pv`,
            forceWrite: true,
          }
        )
      );

      cy.wrapFn(
        () => porcupineService.start()
      );

      cy.mockRecording(`audio_samples/${testInfo.wakeword.replaceAll(' ', '_')}.wav`);

      cy.wrapFn(
        () => porcupineService.stop()
      );

      cy.wrapFn(
        () => porcupineService.release()
      );

      let i = 0;
      const expected = [true, false];

      porcupineService.isLoaded$.subscribe(isLoaded => {
        expect(isLoaded).to.be.true;
      });

      porcupineService.isListening$.subscribe(isListening => {
        expect(isListening).to.eq(expected[i++]);
        if (i == expected.length) {
          done();
        }
      });

      porcupineService.keywordDetection$.subscribe(keyword => {
        expect(keyword.label).to.be.eq(testInfo.wakeword);
      });
    });
  }
});

