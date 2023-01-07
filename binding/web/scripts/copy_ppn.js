const fs = require("fs");
const { join } = require("path");

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
