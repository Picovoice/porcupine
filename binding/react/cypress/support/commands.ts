import { WebVoiceProcessor } from '@picovoice/web-voice-processor';
import { act } from '@testing-library/react-hooks/dom';

Cypress.Commands.add("wrapHook", (fn: () => Promise<any>) => {
  return cy.wrap(null).then(async () => {
    await act(async () => {
      await fn();
    });
  });
});

Cypress.Commands.add("mockRecording", (path: string, delayMs = 1000) => {
  // @ts-ignore
  const instance = WebVoiceProcessor.instance();

  instance._microphoneStream?.getAudioTracks().forEach((track: any) => {
    track.enabled = false;
  });

  cy.fixture(path, 'base64').then(Cypress.Blob.base64StringToBlob).then(async blob => {
    const data = new Int16Array(await blob.arrayBuffer());
    for (let i = 0; i < data.length; i += 512) {
      instance.recorderCallback(data.slice(i, i + 512));
    }
  }).wait(delayMs);

  instance._microphoneStream?.getAudioTracks().forEach((track: any) => {
    track.enabled = true;
  });
});
