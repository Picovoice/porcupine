import fs from "fs";
import ncp from "ncp";
import { dirname, join } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));

for (const language of ["en"]) {
  for (const flavour of ["factory"]) {
    console.log(`Template: ${language} ${flavour}`);

    // Workers
    // 1. Copy test files into applicable project
    const projectRootPath = join(__dirname, "..");
    const testFile = join(
      projectRootPath,
      "tests",
      `${language}-${flavour}`
    );
    const projectLocation = join(
      projectRootPath,
      `porcupine-web-${language}-${flavour}`,
      "test"
    );

    // Create the output directory structure, if it doesn't exist
    fs.mkdirSync(projectLocation, { recursive: true });

    ncp(testFile, projectLocation, (err) => {
      if (err) {
        console.error(err);
      }
    });
  }
}
