import fs, { readFileSync, writeFileSync } from "fs";
import { dirname, join } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));

const LANGUAGES = ["en", "de", "es", "fr"];

console.log("Updating the WASM models...");

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

  try {
    fs.mkdirSync(outputDirectory, { recursive: true });

    const wasmFile = readFileSync(
      join(sourceDirectory, "pv_porcupine.wasm")
    );
    const strBase64 = Buffer.from(wasmFile).toString("base64");
    const jsSourceFileOutput = `export const PORCUPINE_WASM_BASE64 = '${strBase64}';\n`;
  
    writeFileSync(join(outputDirectory, "porcupine_b64.ts"), jsSourceFileOutput);
  } catch (error) {
    console.error(error);
  }
}

console.log("... Done!");
