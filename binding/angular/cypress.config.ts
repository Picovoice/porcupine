import { defineConfig } from "cypress";

export default defineConfig({
  component: {
    devServer: {
      framework: "angular",
      bundler: "webpack"
    },
    supportFile: "cypress/support/index.ts",
    specPattern: "test/*.test.{js,jsx,ts,tsx}",
    video: false,
    screenshotOnRunFailure: false
  },
});
