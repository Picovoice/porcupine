import { renderHook } from '@testing-library/react-hooks/dom';
import { BuiltInKeyword } from '@picovoice/porcupine-web';

import { usePorcupine } from '../src';

// @ts-ignore
import porcupineParams from '@/porcupine_params.js';

import testData from './test_data.json';

const ACCESS_KEY = Cypress.env('ACCESS_KEY');

describe('Porcupine binding', () => {
  it('should be able to init via public path', () => {
    const { result } = renderHook(() => usePorcupine());

    cy.wrapHook(
      () => result.current.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { publicPath: "/test/porcupine_params.pv", forceWrite: true }
      )
    ).then(() => {
      expect(result.current.isLoaded, `Failed to load 'porcupine_params.pv' with ${result.current.error}`).to.be.true;
    });

    cy.wrapHook(
      result.current.release
    ).then(() => {
      expect(result.current.isLoaded, `Failed to release porcupine with ${result.current.error}`).to.be.false;
    });
  });

  it('should be able to init via base64', () => {
    const { result } = renderHook(() => usePorcupine());

    cy.wrapHook(
      () => result.current.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { base64: porcupineParams, forceWrite: true }
      )
    ).then(() => {
      expect(result.current.isLoaded, `Failed to load 'porcupine_params.js' with ${result.current.error}`).to.be.true;
    });
  });

  it('should show invalid model path error message', () => {
    const { result } = renderHook(() => usePorcupine());

    cy.wrapHook(
      () => result.current.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { publicPath: "/porcupine_params_failed.pv", forceWrite: true }
      )
    ).then(() => {
      expect(result.current.isLoaded).to.be.false;
      expect(result.current.error?.toString()).to.contain("Error response returned while fetching model from '/porcupine_params_failed.pv'");
    });
  });

  it('should show invalid access key error message', () => {
    const { result } = renderHook(() => usePorcupine());

    cy.wrapHook(
      () => result.current.init(
        '',
        BuiltInKeyword.Porcupine,
        { publicPath: "/test/porcupine_params.pv", forceWrite: true }
      )
    ).then(() => {
      expect(result.current.isLoaded).to.be.false;
      expect(result.current.error?.toString()).to.contain("Invalid AccessKey");
    });
  });

  for (const testInfo of testData.tests.singleKeyword) {
    it(`should be able to process audio (${testInfo.language})`, () => {
      const { result } = renderHook(() => usePorcupine());

      cy.wrapHook(
        () => result.current.init(
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
        expect(result.current.isLoaded, `Failed to load ${testInfo.wakeword} (${testInfo.language}) with ${result.current.error}`).to.be.true;
      });

      cy.wrapHook(
        result.current.start
      ).then(() => {
        expect(result.current.isListening, `Failed to start processing with ${result.current.error}`).to.be.true;
      });

      cy.mockRecording(`audio_samples/${testInfo.wakeword.replaceAll(' ', '_')}.wav`).then(() => {
        expect(result.current.keywordDetection?.label).to.be.eq(testInfo.wakeword);
      });

      cy.wrapHook(
        result.current.stop
      ).then(() => {
        expect(result.current.isListening, `Failed to stop processing with ${result.current.error}`).to.be.false;
      });
    });
  }
});
