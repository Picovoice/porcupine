import { defineConfig } from "cypress";

export default defineConfig({
  env: {
    "NUM_TEST_ITERATIONS": 15,
    "INIT_PERFORMANCE_THRESHOLD_SEC": 0.5,
    "PROC_PERFORMANCE_THRESHOLD_SEC": 0.65
  },
  e2e: {
    defaultCommandTimeout: 30000,
    supportFile: "cypress/support/index.ts",
    specPattern: "test/*.test.{js,jsx,ts,tsx}",
    video: false,
    screenshotOnRunFailure: false,
  },
});
