const fs = require('fs');
const { join } = require('path');

console.log('Copying the porcupine models...');

const fixturesDirectory = join(__dirname, '..', 'cypress', 'fixtures');
const staticDirectory = join(__dirname, '..', 'static');
const testDirectory = join(__dirname, '..', 'test');

const paramsSourceDirectory = join(
  __dirname,
  '..',
  '..',
  '..',
  'lib',
  'common',
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


console.log("Copying the PPN model...");

const sourceDirectory = join(
  __dirname,
  "..",
  "..",
  "..",
  "resources",
);

try {
  fs.mkdirSync(join(staticDirectory), { recursive: true });
  fs.readdirSync(paramsSourceDirectory).forEach(file => {
    fs.copyFileSync(join(paramsSourceDirectory, file), join(staticDirectory, file));
  });

  fs.copyFileSync(testDataSource, join(testDirectory, 'test_data.json'));

  fs.mkdirSync(join(staticDirectory, 'keywords'), { recursive: true });
  fs.readdirSync(sourceDirectory).forEach(folder => {
    if (folder.includes("keyword_files")) {
      fs.readdirSync(join(sourceDirectory, folder, 'wasm')).forEach(file => {
        fs.copyFileSync(join(sourceDirectory, folder, 'wasm', file), join(staticDirectory, 'keywords', file));
      })
    }
  });

  fs.mkdirSync(join(fixturesDirectory, 'audio_samples'), { recursive: true });
  fs.readdirSync(join(sourceDirectory, 'audio_samples')).forEach(file => {
    fs.copyFileSync(join(sourceDirectory, 'audio_samples', file), join(fixturesDirectory, 'audio_samples', file));
  });
} catch (error) {
  console.error(error);
}

console.log('... Done!');
