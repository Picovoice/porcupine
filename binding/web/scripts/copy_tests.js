const ncp = require("ncp");
const path = require("path");
const editJsonFile = require("edit-json-file");

for (const language of ["en"]) {
  for (const flavour of ["factory"]) {
    console.log(`Template: ${language} ${flavour}`);

    // Workers
    // 1. Copy test files into applicable project
    const projectRootPath = path.join(__dirname, "..");
    const testFile = path.join(
      projectRootPath,
      "test",
      `porcupine-web-${language}-${flavour}`
    );
    const projectLocation = path.join(
      projectRootPath,
      `porcupine-web-${language}-${flavour}`,
      "test"
    );

    ncp(testFile, projectLocation, (err) => {
      if (err) {
        console.error(err);
      }
    });
  }
}
