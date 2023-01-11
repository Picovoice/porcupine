import { defineConfig } from "cypress";
import * as path from "path";

const CopyWebpackPlugin = require('copy-webpack-plugin');

export default defineConfig({
  component: {
    devServer: {
      framework: "angular",
      bundler: "webpack",
      webpackConfig: {
        context: path.join(__dirname),
        plugins: [
          new CopyWebpackPlugin({
            patterns: [
              { from: 'static' }
            ]
          })
        ],
        module: {
          rules: [
            {
              test: /(test|cypress)\/.*\.ts$/,
              use: [{
                loader: 'ts-loader',
                options: {
                  configFile: 'cypress/tsconfig.json'
                }
              }],
              exclude: /node_modules/,
            },
          ]
        }
      }
    },
    supportFile: "cypress/support/index.ts",
    specPattern: "test/*.test.ts",
    video: false,
    screenshotOnRunFailure: false,
  },
});
