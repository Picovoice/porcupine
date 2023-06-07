const child_process = require("child_process");
const fs = require("fs");
const path = require("path");
const testData = require("../../../resources/.test/test_data.json");

const availableLanguages = testData["tests"]["singleKeyword"].map(
  (x) => x["language"]
);

const commands = process.argv.slice(2, -1);
const language = process.argv.slice(-1)[0];

if (!availableLanguages.includes(language)) {
  console.error(
    `Choose the language you would like to run the demo in with "yarn start [language]".\nAvailable languages are ${availableLanguages.join(
      ", "
    )}`
  );
  process.exit(1);
}

const version = process.env.npm_package_version;
const suffix = language === "en" ? "" : `_${language}`;
const rootDir = path.join(__dirname, "..", "..", "..");

const keywordDir = path.join(
  rootDir,
  "resources",
  `keyword_files${suffix}`,
  "wasm"
);

const libDirectory = path.join(__dirname, "..", "src", "lib");
let publicDirectory = path.join(__dirname, "..", "src", "assets", "keywords");
if (fs.existsSync(publicDirectory)) {
  fs.readdirSync(publicDirectory).forEach((k) => {
    fs.unlinkSync(path.join(publicDirectory, k));
  });
} else {
  fs.mkdirSync(publicDirectory, { recursive: true });
}

const keywordJS = [];
if (language !== "en") {
  try {
    const keywords = fs
      .readdirSync(keywordDir)
      .filter((f) => path.extname(f) === ".ppn");

    keywords.forEach((k) => {
      fs.copyFileSync(path.join(keywordDir, k), path.join(publicDirectory, k));
      keywordJS.push(`  {
    label: "${k.replace("_wasm.ppn", "").replace("_", " ")}",
    sensitivity: 0.7,
    publicPath: "assets/keywords/${k}",
    customWritePath: "${version}_${k}",
  },`);
    });
  } catch (error) {
    console.error(error);
    process.exit(1);
  }
}

fs.writeFileSync(
  path.join(libDirectory, "porcupineKeywords.js"),
  `const porcupineKeywords = [
${keywordJS.join("\n")}
];

(function () {
  if (typeof module !== "undefined" && typeof module.exports !== "undefined")
    module.exports = porcupineKeywords;
})();`
);

const modelDir = path.join(rootDir, "lib", "common");

publicDirectory = path.join(__dirname, "..", "src", "assets", "models");
if (fs.existsSync(publicDirectory)) {
  fs.readdirSync(publicDirectory).forEach((k) => {
    fs.unlinkSync(path.join(publicDirectory, k));
  });
} else {
  fs.mkdirSync(publicDirectory, { recursive: true });
}

const modelName = `porcupine_params${suffix}.pv`;
fs.copyFileSync(
  path.join(modelDir, modelName),
  path.join(publicDirectory, modelName)
);

fs.writeFileSync(
  path.join(libDirectory, "porcupineModel.js"),
  `const porcupineModel = {
  publicPath: "assets/models/${modelName}",
  customWritePath: "${version}_${modelName}",
};

(function () {
  if (typeof module !== "undefined" && typeof module.exports !== "undefined")
    module.exports = porcupineModel;
})();`
);

child_process.fork("ng", commands, {
  execPath: "npx",
});
