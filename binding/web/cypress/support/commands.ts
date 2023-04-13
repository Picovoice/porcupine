
const WAV_HEADER_SIZE = 44;

Cypress.Commands.add("getFramesFromFile", (path: string) => {
  cy.fixture(path, 'base64').then(Cypress.Blob.base64StringToBlob).then(async blob => {
    const data = new Int16Array(await blob.arrayBuffer());
    return data.slice(WAV_HEADER_SIZE / Int16Array.BYTES_PER_ELEMENT);
  });
});
