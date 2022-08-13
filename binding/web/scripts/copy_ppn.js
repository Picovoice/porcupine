const fs = require("fs");
const { join } = require("path");

const ppnFiles = [
  "alexa_wasm.ppn",
  "americano_wasm.ppn",
  "blueberry_wasm.ppn",
  "bumblebee_wasm.ppn",
  "computer_wasm.ppn",
  "grapefruit_wasm.ppn",
  "grasshopper_wasm.ppn",
  "hey google_wasm.ppn",
  "hey siri_wasm.ppn",
  "jarvis_wasm.ppn",
  "ok google_wasm.ppn",
  "picovoice_wasm.ppn",
  "porcupine_wasm.ppn",
  "terminator_wasm.ppn",
]

console.log("Copying the PPN model...");

const sourceDirectory = join(
  __dirname,
  "..",
  "..",
  "..",
  "resources",
  "keyword_files",
  "wasm"
);

const outputDirectory = join(__dirname, "..", "keywords");

try {
  fs.mkdirSync(outputDirectory, { recursive: true });
  ppnFiles.forEach(file => {
    fs.copyFileSync(join(sourceDirectory, file), join(outputDirectory, file))
  })
} catch (error) {
  console.error(error);
}

console.log("... Done!");
