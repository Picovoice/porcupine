import "./commands";

declare global {
  namespace Cypress {
    interface Chainable {
      mockRecording(path: string, delayMs?: number): Chainable<void>;
    }
  }
}
