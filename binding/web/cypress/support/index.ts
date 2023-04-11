import "./commands";

declare global {
  namespace Cypress {
    interface Chainable {
      getFramesFromFile(path: string): Chainable<Int16Array>;
    }
  }
}
