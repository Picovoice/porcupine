import { dirname, join } from "path";
import { fileURLToPath } from "url";
import ncp from "ncp";

const __dirname = dirname(fileURLToPath(import.meta.url));

const LANGUAGES = ["en", "de", "es", "fr"];

console.log("Copying wasm/JS code from /lib/wasm/ ...");

for (const language of LANGUAGES) {
  console.log(`--- Language: '${language}' ---`);
  const sourceDirectory = join(
    __dirname,
    "..",
    "..",
    "..",
    "lib",
    "wasm",
    language
  );

  const outputDirectory = join(__dirname, "..", language);

  ncp(sourceDirectory, outputDirectory, (error) => {
    if (error) {
      console.error(error);
    } else {
      console.log(`Copied ${sourceDirectory} => ${outputDirectory}`);
    }
  });
}

console.log("... Done!");
