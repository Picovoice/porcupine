const fs = require("fs");
const { join } = require("path");

const wasmFiles = ["pv_porcupine.wasm", "pv_porcupine_simd.wasm"]

console.log("Copying the WASM model...");

const sourceDirectory = join(
  __dirname,
  "..",
  "..",
  "..",
  "lib",
  "wasm"
);

const outputDirectory = join(__dirname, "..", "lib");

try {
  fs.mkdirSync(outputDirectory, { recursive: true });
  wasmFiles.forEach(file => {
    fs.copyFileSync(join(sourceDirectory, file), join(outputDirectory, file))
  })
} catch (error) {
  console.error(error);
}

console.log("... Done!");
