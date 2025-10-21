import { defineConfig } from "cypress";
import viteConfig from "./vite.config.js";

export default defineConfig({
  component: {
    devServer: {
      framework: "react",
      bundler: "vite",
      viteConfig,
    },
    supportFile: "cypress/support/index.ts",
    specPattern: "test/*.test.{js,jsx,ts,tsx}",
    video: false,
    screenshotOnRunFailure: false,
  },
});
