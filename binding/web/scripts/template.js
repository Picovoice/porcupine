const ncp = require("ncp");
const path = require("path");

for (const language of ["en", "de"]) {
  console.log(language);

  // 1. Copy language-agnostic project: i.e., the 'template' folder, to target (e.g. porcupine-web-en-worker)

  let projectRootPath = path.join(__dirname, "..");
  let templateDirectory = path.join(projectRootPath, "template");
  let buildTarget = `porcupine-web-${language}-worker`;
  let outputDirectory = path.join(projectRootPath, buildTarget);
  console.log(1);
  ncp(templateDirectory, outputDirectory, (err) => {
    console.log(err);
  });

  // 2. Copy language-specific features (keywords, Emscripten)
  console.log(2);
  const languageDirectory = path.join(projectRootPath, language);
  ncp(languageDirectory, path.join(outputDirectory, "src", "lang"), (err) => {
    console.log(err);
  });

  // 3. Customize the package.json to have the correct target name
}
