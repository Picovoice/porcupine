import { BuiltInKeyword } from '@picovoice/porcupine-web';

import { usePorcupine } from '../src/porcupine';

// @ts-ignore
import porcupineParams from '@/porcupine_params.js';
import testData from './test_data.json';

const ACCESS_KEY = Cypress.env('ACCESS_KEY');

describe('Porcupine binding', () => {
  it('should be able to init via public path', () => {
    const ppn = usePorcupine();

    cy.wrapFn(
      () => ppn.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { publicPath: "/test/porcupine_params.pv", forceWrite: true }
      )
    ).then(() => {
      expect(ppn.state.isLoaded).to.be.true;
    });

    cy.wrapFn(
      ppn.release
    ).then(() => {
      expect(ppn.state.isLoaded).to.be.false;
    });
  });

  it('should be able to init via base64', () => {
    const ppn = usePorcupine();

    cy.wrapFn(
      () => ppn.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { base64: porcupineParams, forceWrite: true }
      )
    ).then(() => {
      expect(ppn.state.isLoaded).to.be.true;
    });

    cy.wrapFn(ppn.release);
  });

  it('should show invalid model path error message', () => {
    const ppn = usePorcupine();

    cy.wrapFn(
      () => ppn.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { publicPath: "/porcupine_params_failed.pv", forceWrite: true }
      )
    ).then(() => {
      expect(ppn.state.isLoaded).to.be.false;
      expect(ppn.state.error?.toString()).to.contain("Error response returned while fetching model from '/porcupine_params_failed.pv'");
    });

    cy.wrapFn(ppn.release);
  });

  it('should show invalid access key error message', () => {
    const ppn = usePorcupine();

    cy.wrapFn(
      () => ppn.init(
        '',
        BuiltInKeyword.Porcupine,
        { publicPath: "/test/porcupine_params.pv", forceWrite: true }
      )
    ).then(() => {
      expect(ppn.state.isLoaded).to.be.false;
      expect(ppn.state.error?.toString()).to.contain("Invalid AccessKey");
    });

    cy.wrapFn(ppn.release);
  });

  for (const testInfo of testData.tests.singleKeyword) {
    it(`should be able to process audio (${testInfo.language})`, () => {
      const ppn = usePorcupine();

      cy.wrapFn(
        () => ppn.init(
          ACCESS_KEY,
          {
            label: testInfo.wakeword,
            publicPath: `/test/keywords/${testInfo.wakeword}_wasm.ppn`,
            forceWrite: true,
          },
          {
            publicPath: testInfo.language === 'en' ? "/test/porcupine_params.pv" : `/test/porcupine_params_${testInfo.language}.pv`,
            forceWrite: true,
          }
        )
      ).then(() => {
        expect(ppn.state.isLoaded).to.be.true;
      });

      cy.wrapFn(
        ppn.start
      ).then(() => {
        expect(ppn.state.isListening).to.be.true;
      });

      cy.mockRecording(`audio_samples/${testInfo.wakeword.replaceAll(' ', '_')}.wav`).then(() => {
        expect(ppn.state.keywordDetection?.label).to.be.eq(testInfo.wakeword);
      });

      cy.wrapFn(
        ppn.stop
      ).then(() => {
        expect(ppn.state.isListening).to.be.false;
      });

      cy.wrapFn(ppn.release);
    });
  }
});
