const fs = require("fs");
const path = require("path");

const LANGUAGES = ["en", "de", "es", "fr"];

const BUILT_IN_WAKE_WORDS_EN = [
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

const BUILT_IN_WAKE_WORDS_DE = [
  "ananas",
  "heuschrecke",
  "himbeere",
  "leguan",
  "stachelschwein",
];

const BUILT_IN_WAKE_WORDS_FR = [
  "framboise",
  "mon chouchou",
  "parapluie",
  "perroquet",
  "tournesol",
];

const BUILT_IN_WAKE_WORDS_ES = [
  "emparedado",
  "leopardo",
  "manzana",
  "murciélago",
];

const SOURCE_DIRECTORY_MAP = new Map();
SOURCE_DIRECTORY_MAP.set("de", path.join("keyword_files_de", "wasm"));
SOURCE_DIRECTORY_MAP.set("en", path.join("keyword_files", "wasm"));
SOURCE_DIRECTORY_MAP.set("es", path.join("keyword_files_es", "wasm"));
SOURCE_DIRECTORY_MAP.set("fr", path.join("keyword_files_fr", "wasm"));

const LANGUAGE_WAKE_WORDS = new Map();
LANGUAGE_WAKE_WORDS.set("de", BUILT_IN_WAKE_WORDS_DE);
LANGUAGE_WAKE_WORDS.set("en", BUILT_IN_WAKE_WORDS_EN);
LANGUAGE_WAKE_WORDS.set("es", BUILT_IN_WAKE_WORDS_ES);
LANGUAGE_WAKE_WORDS.set("fr", BUILT_IN_WAKE_WORDS_FR);

/**
 * Convert all of the Porcupine .ppn files to base64,
 * so they can be easily inlined into JavaScript
 */
console.log("Converting keyword files (.ppn) to Base64 ...");
for (const language of LANGUAGES) {
  console.log(`--- Language: '${language}' ---`);
  let sourceDirectory = path.join(
    __dirname,
    "..",
    "..",
    "..",
    "resources",
    SOURCE_DIRECTORY_MAP.get(language)
  );

  let outputDirectory = path.join(__dirname, "..", language, "dist");

  // Convert all WASM PPN binary model files to base64 strings;
  // add a little JavaScript to export them as ES modules
  let jsSourceFileOutput = "";
  for (const wakeWord of LANGUAGE_WAKE_WORDS.get(language)) {
    console.log(wakeWord);
    let ppnModel = fs.readFileSync(
      path.join(sourceDirectory, `${wakeWord}_wasm.ppn`)
    );
    let strBase64 = Buffer.from(ppnModel).toString("base64");
    let keywordName = wakeWord.replace(" ", "_").replace("é", "e");
    jsSourceFileOutput += `export const ${keywordName.toUpperCase()}_64 = "${strBase64}"\n`;
  }

  fs.writeFileSync(
    path.join(outputDirectory, `built_in_keywords_64.ts`),
    jsSourceFileOutput
  );
}

console.log("... Done!");
