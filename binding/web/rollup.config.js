'use strict';
const path = require('path');
const { nodeResolve } = require('@rollup/plugin-node-resolve');
const commonjs = require('@rollup/plugin-commonjs');
const typescript = require('rollup-plugin-typescript2');
const workerLoader = require('rollup-plugin-web-worker-loader');
const pkg = require('./package.json');
const { babel } = require('@rollup/plugin-babel');
const terser = require('rollup-plugin-terser').terser;
const { DEFAULT_EXTENSIONS } = require('@babel/core');

const extensions = [...DEFAULT_EXTENSIONS, '.ts'];

console.log(process.env.TARGET);
console.log(extensions);

export default {
  input: [path.resolve(__dirname, pkg.entry)],
  output: [
    {
      file: path.resolve(__dirname, pkg['module']),
      format: 'esm',
      sourcemap: false,
    },
    {
      file: path.resolve(__dirname, 'dist', 'esm', 'index.min.js'),
      format: 'esm',
      sourcemap: false,
      plugins: [terser()],
    },
    {
      file: path.resolve(__dirname, pkg.iife),
      format: 'iife',
      name: 'PorcupineWebEnWorker',
      sourcemap: false,
    },
    {
      file: path.resolve(__dirname, 'dist', 'iife', 'index.min.js'),
      format: 'iife',
      name: 'PorcupineWebEnWorker',
      sourcemap: false,
      plugins: [terser()],
    },
  ],
  plugins: [
    nodeResolve({ extensions }),
    commonjs(),
    workerLoader({ targetPlatform: 'browser', sourcemap: false }),
    typescript({
      typescript: require('typescript'),
      cacheRoot: path.resolve(__dirname, '.rts2_cache'),
      clean: true,
    }),
    babel({
      extensions: extensions,
      babelHelpers: 'runtime',
      exclude: '**/node_modules/**',
    }),
  ],
};
