const fs = require('fs');
const { join } = require('path');

console.log('Copying the porcupine test files...');

const outputDirectory = join(__dirname, '..', 'test');
const fixtureDirectory = join(__dirname, '..', 'cypress', 'fixtures');

const paramsSourceDirectory = join(
  __dirname,
  '..',
  '..',
  '..',
  'lib',
  'common',
);

const audioPathDirectory = join(
  __dirname,
  '..',
  '..',
  '..',
  'resources',
  'audio_samples'
)

try {
  fs.mkdirSync(outputDirectory, { recursive: true });

  fs.copyFileSync(join(paramsSourceDirectory, 'porcupine_params.pv'), join(outputDirectory, 'porcupine_params.pv'));
  fs.copyFileSync(join(audioPathDirectory, 'porcupine.wav'), join(fixtureDirectory, 'porcupine.wav'))
} catch (error) {
  console.error(error);
}

console.log('... Done!');
