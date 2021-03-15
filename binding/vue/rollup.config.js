'use strict';
const path = require('path');
const { nodeResolve } = require('@rollup/plugin-node-resolve');
const commonjs = require('@rollup/plugin-commonjs');
const pkg = require('./package.json');
const { babel } = require('@rollup/plugin-babel');
const terser = require('rollup-plugin-terser').terser;
const vue = require('rollup-plugin-vue');
const { DEFAULT_EXTENSIONS } = require('@babel/core');

const extensions = DEFAULT_EXTENSIONS;

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
  external: ['vue'],
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
      name: iifeBundleName,
      sourcemap: false,
      globals: {
        vue: 'Vue',
      },
    },
    {
      file: path.resolve(__dirname, 'dist', 'iife', 'index.min.js'),
      format: 'iife',
      name: iifeBundleName,
      sourcemap: false,
      plugins: [terser()],
      globals: {
        vue: 'Vue',
      },
    },
  ],
  plugins: [
    vue(),
    nodeResolve({ extensions }),
    commonjs(),
    babel({
      extensions: extensions,
      babelHelpers: 'runtime',
      exclude: '**/node_modules/**',
    }),
  ],
};
