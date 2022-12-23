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
);

const outputDirectory = join(__dirname, "..", "keywords");

try {
  fs.mkdirSync(outputDirectory, { recursive: true });

  fs.readdirSync(sourceDirectory).forEach(folder => {
    if (folder.includes("keyword_files")) {
      fs.readdirSync(join(sourceDirectory, folder, 'wasm')).forEach(file => {
        fs.copyFileSync(join(sourceDirectory, folder, 'wasm', file), join(outputDirectory, file));
      })
    }
  });
} catch (error) {
  console.error(error);
}

console.log("... Done!");
