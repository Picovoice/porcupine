# porcupine-web-template

This is a template package for building the porcupine-web-\* projects. Each language requires a separate NPM package due to the size of the payload included.

The code in each package is largely identical. Using this template you can create all of these projects dynamically from the template folder.

## Create porcupine-web-\* projects

Use `yarn` then `yarn build` to gather dependencies, convert built-in Porcupine `.ppn` keyword files to base64 format (and JS code to wrap them), and generate a project per language from the project template:

```console
yarn
yarn build
```

Now each individual project will exist. E.g.:

```console
cd porcupine-web-en-worker
yarn
yarn build
```
