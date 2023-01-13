import "./commands";

declare global {
  namespace Cypress {
    interface Chainable {
      wrapFn(fn: () => Promise<any>): Chainable<void>;
      mockRecording(path: string, delayMs?: number): Chainable<void>;
    }
  }
}
