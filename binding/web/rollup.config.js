'use strict';
const path = require('path');
const { nodeResolve } = require('@rollup/plugin-node-resolve');
const commonjs = require('@rollup/plugin-commonjs');
const typescript = require('rollup-plugin-typescript2');
const workerLoader = require('rollup-plugin-web-worker-loader');
const pkg = require('./package.json');
const { babel } = require('@rollup/plugin-babel');
const terser = require('@rollup/plugin-terser');
const { DEFAULT_EXTENSIONS } = require('@babel/core');
const { base64 } = require('@picovoice/web-utils/plugins');

const extensions = [...DEFAULT_EXTENSIONS, '.ts'];

console.log(process.env.TARGET);
console.log(extensions);

function capitalizeFirstLetter(string) {
  return string.charAt(0).toUpperCase() + string.slice(1);
}

const iifeBundleName = pkg.name
  .split('@picovoice/')[1]
  .split('-')
  .map(word => capitalizeFirstLetter(word))
  .join('');
console.log(iifeBundleName);

export default {
  input: [path.resolve(__dirname, pkg.entry)],
  output: [
    {
      file: path.resolve(__dirname, pkg.module),
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
      name: iifeBundleName,
      sourcemap: false,
    },
    {
      file: path.resolve(__dirname, 'dist', 'iife', 'index.min.js'),
      format: 'iife',
      name: iifeBundleName,
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
    base64({
      include: ['./lib/**/*.wasm', './keywords/**/*.ppn'],
    })
  ],
};
