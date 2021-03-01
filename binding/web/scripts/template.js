const ncp = require("ncp");
const path = require("path");
const editJsonFile = require("edit-json-file");

for (const language of ["en","de"]) {
  console.log("Template: " + language);

  // 1. Copy language-agnostic project: i.e., the 'template' folder, to target
  // (e.g. porcupine-web-en-worker)
  const projectRootPath = path.join(__dirname, "..");
  const templateDirectory = path.join(projectRootPath, "template");
  const buildTarget = `porcupine-web-${language}-worker`;
  const outputDirectory = path.join(projectRootPath, buildTarget);
  ncp(templateDirectory, outputDirectory, (err) => {
    console.error("ncp done");
    if (err) {
      console.error(err);
    } else {
      // 2. Copy language-specific features (built-in keywords, Emscripten)
      const languageDirectory = path.join(projectRootPath, language);
      ncp(
        languageDirectory,
        path.join(outputDirectory, "src", "lang"),
        (err) => {
          if (err) {
            console.error(error);
          }
          // 3. Customize the package.json to have the correct names and build targets
          const packageJson = editJsonFile(
            path.join(outputDirectory, "package.json")
          );
          packageJson.set("name", buildTarget);
          packageJson.save((e) => {
            console.log("Package JSON updated");
          });
        }
      );
    }
  });
}
