import { defineConfig } from "cypress";

export default defineConfig({
  component: {
    devServer: {
      framework: "vue",
      bundler: "vite",
    },
    supportFile: "cypress/support/index.ts",
    specPattern: "test/*.test.{js,jsx,ts,tsx}",
    video: false,
    screenshotOnRunFailure: false
  },
});
