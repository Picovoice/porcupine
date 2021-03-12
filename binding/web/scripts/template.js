const ncp = require("ncp");
const path = require("path");
const editJsonFile = require("edit-json-file");

for (const language of ["en", "de"]) {
  for (const flavour of ["factory", "worker"]) {
    console.log(`Template: ${language} ${flavour}`);

    // Workers
    // 1. Copy language-agnostic project: i.e., the 'template' folder, to target
    // (e.g. porcupine-web-en-worker)
    const projectRootPath = path.join(__dirname, "..");
    const templateDirectory = path.join(projectRootPath, "template");
    const buildTarget = `porcupine-web-${language}-${flavour}`;
    const outputDirectory = path.join(projectRootPath, buildTarget);
    ncp(templateDirectory, outputDirectory, (err) => {
      console.log(`${language}: ncp done`);
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
            } else {
              // 4. README?

              // 5. index.ts: Rollup's entry point is different for workers/factories
              console.log(path.join(projectRootPath, flavour, "index.ts"));
              console.log(path.join(outputDirectory, "src"));
              ncp(
                path.join(projectRootPath, flavour, "index.ts"),
                path.join(outputDirectory, "src", "index.ts"),
                (err) => {
                  if (err) {
                    console.error(error);
                  } else {
                    console.log("index.ts copied");

                    // 3. Customize the package.json to have the correct names and build targets
                    const packageJson = editJsonFile(
                      path.join(outputDirectory, "package.json")
                    );
                    packageJson.set("name", `@picovoice/${buildTarget}`);
                    packageJson.save((e) => {
                      console.log(`${buildTarget} Package JSON updated`);
                    });
                  }
                }
              );
            }
          }
        );
      }
    });
  }
}
