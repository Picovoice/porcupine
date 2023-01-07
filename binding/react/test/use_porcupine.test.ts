import { act, renderHook } from '@testing-library/react-hooks/dom';
import { BuiltInKeyword } from '@picovoice/porcupine-web';

import { usePorcupine } from '../src';

// @ts-ignore
import porcupineParams from '@/porcupine_params.js';


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
      expect(result.current.isLoaded).to.be.true;
    });

    cy.wrapHook(
      result.current.release
    ).then(() => {
      expect(result.current.isLoaded).to.be.false;
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
      expect(result.current.isLoaded).to.be.true;
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
      expect(result.current.error).to.contain("Error response returned while fetching model from '/porcupine_params_failed.pv'");
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
      expect(result.current.error).to.contain("Invalid AccessKey");
    });
  });

  it('should be able to process audio', () => {
    const { result } = renderHook(() => usePorcupine());

    cy.wrapHook(
      () => result.current.init(
        ACCESS_KEY,
        BuiltInKeyword.Porcupine,
        { publicPath: "/test/porcupine_params.pv", forceWrite: true }
      )
    ).then(() => {
      expect(result.current.isLoaded).to.be.true;
    });

    cy.wrapHook(
      result.current.start
    ).then(() => {
      expect(result.current.isListening).to.be.true;
    });

    cy.mockRecording('porcupine.wav').then(() => {
      expect(result.current.keywordDetection?.label).to.be.eq('Porcupine');
    });

    cy.wrapHook(
      result.current.stop
    ).then(() => {
      expect(result.current.isListening).to.be.false;
    });
  });
});
