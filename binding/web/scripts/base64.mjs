import fs, {readFileSync, writeFileSync} from "fs";
import {dirname, join} from "path";
import {fileURLToPath} from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));

const BUILT_IN_WAKE_WORDS = [
  "alexa",
  "americano",
  "blueberry",
  "bumblebee",
  "computer",
  "grapefruit",
  "grasshopper",
  "hey google",
  "hey siri",
  "jarvis",
  "ok google",
  "picovoice",
  "porcupine",
  "terminator",
];

/**
 * Convert all of the Porcupine .ppn files to base64,
 * so they can be easily inlined into JavaScript
 */
console.log("Converting keyword files (.ppn) to Base64 ...");

const sourceDirectory = join(
  __dirname,
  "..",
  "..",
  "..",
  "resources",
  join("keyword_files", "wasm")
);

const outputDirectory = join(__dirname, "..", "src");

// Create the output directory structure, if it doesn't exist
fs.mkdirSync(outputDirectory, {recursive: true});

// Convert all WASM PPN binary model files to base64 strings;
// add a little JavaScript to export them as ES modules
let jsSourceFileOutput = "";
for (const wakeWord of BUILT_IN_WAKE_WORDS) {
  console.log(wakeWord);
  const ppnModel = readFileSync(
    join(sourceDirectory, `${wakeWord}_wasm.ppn`)
  );
  const strBase64 = Buffer.from(ppnModel).toString("base64");
  const keywordName = wakeWord.replace(" ", "_").replace("é", "e");
  jsSourceFileOutput += `export const ${keywordName.toUpperCase()}_64 = "${strBase64}"\n`;
}

writeFileSync(
  join(outputDirectory, `built_in_keywords_64.ts`),
  jsSourceFileOutput
);

console.log("... Done!");
