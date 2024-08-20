const child_process = require('child_process');
const fs = require('fs');
const path = require('path');
const testData = require('../../../resources/.test/test_data.json');

const availableLanguages = testData.tests.singleKeyword.map((x) => x.language);

const args = process.argv.slice(2, -1);
const language = process.argv.slice(-1)[0];

if (!availableLanguages.includes(language)) {
  console.error(
    `Choose the language you would like to run the demo in with "yarn [android/ios]-run [language]".\nAvailable languages are ${availableLanguages.join(
      ', ',
    )}`,
  );
  process.exit(1);
}

const suffix = language === 'en' ? '' : `_${language}`;
const rootDir = path.join(__dirname, '..', '..', '..');

const modelDir = path.join(rootDir, 'lib', 'common');
const keywordDir = path.join(rootDir, 'resources', `keyword_files${suffix}`);

const androidAssetDir = path.join(
  __dirname,
  '..',
  'android',
  'porcupine-rn-demo-app',
  'src',
  'main',
  'assets',
);

const iosBundleDir = path.join(__dirname, '..', 'ios', 'PorcupineDemo');

fs.rmSync(path.join(androidAssetDir, 'models'), {recursive: true, force: true});
fs.rmSync(path.join(androidAssetDir, 'keywords'), {
  recursive: true,
  force: true,
});
fs.rmSync(path.join(iosBundleDir, 'models'), {recursive: true, force: true});
fs.rmSync(path.join(iosBundleDir, 'keywords'), {recursive: true, force: true});

fs.mkdirSync(path.join(androidAssetDir, 'models'), {recursive: true});
fs.mkdirSync(path.join(androidAssetDir, 'keywords'), {recursive: true});
fs.mkdirSync(path.join(iosBundleDir, 'models'), {recursive: true});
fs.mkdirSync(path.join(iosBundleDir, 'keywords'), {recursive: true});

let params = {};

if (language !== 'en') {
  fs.copyFileSync(
    path.join(modelDir, `porcupine_params${suffix}.pv`),
    path.join(androidAssetDir, 'models', `porcupine_params${suffix}.pv`),
  );

  const androidKeywords = fs
    .readdirSync(path.join(keywordDir, 'android'))
    .filter((f) => path.extname(f) === '.ppn');

  androidKeywords.forEach((k) => {
    fs.copyFileSync(
      path.join(keywordDir, 'android', k),
      path.join(androidAssetDir, 'keywords', k),
    );
  });

  fs.copyFileSync(
    path.join(modelDir, `porcupine_params${suffix}.pv`),
    path.join(iosBundleDir, 'models', `porcupine_params${suffix}.pv`),
  );

  const iosKeywords = androidKeywords.map((k) => k.replace('_android', '_ios'));

  iosKeywords.forEach((k) => {
    fs.copyFileSync(
      path.join(keywordDir, 'ios', k),
      path.join(iosBundleDir, 'keywords', k),
    );
  });

  params.language = language;
  params.keywords = androidKeywords.map((k) => k.replace('_android.ppn', ''));
} else {
  params.language = language;
  params.keywords = [];
}

fs.writeFileSync(
  path.join(__dirname, '..', 'params.json'),
  JSON.stringify(params),
);

const command = process.platform === 'win32' ? 'npx.cmd' : 'npx';

child_process.execSync(`${command} react-native ${args.join(" ")}`, {
  shell: true,
  stdio: 'inherit'
});
