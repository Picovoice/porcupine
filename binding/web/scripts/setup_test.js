const fs = require('fs');
const { join } = require('path');
const { createHash } = require('crypto');

console.log('Copying the porcupine models...');

const testDirectory = join(__dirname, '..', 'test');
const fixturesDirectory = join(__dirname, '..', 'cypress', 'fixtures');

const paramsSourceDirectory = join(
  __dirname,
  '..',
  '..',
  '..',
  'lib',
  'common'
);

const testDataSource = join(
  __dirname,
  '..',
  '..',
  '..',
  'resources',
  '.test',
  'test_data.json'
);

console.log('Copying the PPN model...');

const sourceDirectory = join(__dirname, '..', '..', '..', 'resources');

try {
  fs.mkdirSync(testDirectory, { recursive: true });

  fs.readdirSync(paramsSourceDirectory).forEach(file => {
    fs.copyFileSync(
      join(paramsSourceDirectory, file),
      join(testDirectory, file)
    );
  });

  fs.copyFileSync(testDataSource, join(testDirectory, 'test_data.json'));

  fs.mkdirSync(join(testDirectory, 'keywords'), { recursive: true });
  fs.readdirSync(sourceDirectory).forEach(folder => {
    if (folder.includes('keyword_files')) {
      fs.readdirSync(join(sourceDirectory, folder, 'wasm')).forEach(file => {
        let src = join(sourceDirectory, folder, 'wasm', file);

        // Bug in Cypress means we can't read utf-8 file names, so we have to hash them
        let encodedKeywordName = createHash('md5')
          .update(file.replace('_wasm.ppn', ''))
          .digest('hex');
        let dst = join(
          testDirectory,
          'keywords',
          `${encodedKeywordName}_wasm.ppn`
        );
        fs.copyFileSync(src, dst);
      });
    }
  });

  fs.mkdirSync(join(fixturesDirectory, 'audio_samples'), { recursive: true });
  fs.readdirSync(join(sourceDirectory, 'audio_samples')).forEach(file => {
    let src = join(sourceDirectory, 'audio_samples', file);

    // Bug in Cypress means we can't read utf-8 file names, so we have to hash them
    let encodedAudioName = createHash('md5')
      .update(file.replace('.wav', ''))
      .digest('hex');
    let dst = join(
      fixturesDirectory,
      'audio_samples',
      `${encodedAudioName}.wav`
    );
    fs.copyFileSync(src, dst);
  });
} catch (error) {
  console.error(error);
}

console.log('... Done!');
