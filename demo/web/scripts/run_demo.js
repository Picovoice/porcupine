const child_process = require("child_process");
const fs = require("fs");
const path = require("path");
const testData = require("../../../resources/.test/test_data.json");

const availableLanguages = testData["tests"]["singleKeyword"].map(
  (x) => x["language"],
);

const language = process.argv.slice(2)[0];
if (!language) {
  console.error(
    `Choose the language you would like to run the demo in with "yarn start [language]".\nAvailable languages are ${availableLanguages.join(
      ", ",
    )}`,
  );
  process.exit(1);
}

if (!availableLanguages.includes(language)) {
  console.error(
    `'${language}' is not an available demo language.\nAvailable languages are ${availableLanguages.join(
      ", ",
    )}`,
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
  "wasm",
);

let outputDirectory = path.join(__dirname, "..", "keywords");
if (fs.existsSync(outputDirectory)) {
  fs.readdirSync(outputDirectory).forEach((k) => {
    fs.unlinkSync(path.join(outputDirectory, k));
  });
} else {
  fs.mkdirSync(outputDirectory, { recursive: true });
}

const keywordJS = [];
if (language !== "en") {
  try {
    const keywords = fs
      .readdirSync(keywordDir)
      .filter((f) => path.extname(f) === ".ppn");

    keywords.forEach((k) => {
      fs.copyFileSync(path.join(keywordDir, k), path.join(outputDirectory, k));
      keywordJS.push(`  {
    label: "${k.replace("_wasm.ppn", "").replace("_", " ")}",
    sensitivity: 0.7,
    publicPath: "keywords/${k}",    
    customWritePath: "${version}_${k}",
  },`);
    });
  } catch (error) {
    console.error(error);
    process.exit(1);
  }
}

fs.writeFileSync(
  path.join(outputDirectory, "porcupineKeywords.js"),
  `const porcupineKeywords = [
${keywordJS.join("\n")}
];

(function () {
  if (typeof module !== "undefined" && typeof module.exports !== "undefined")
    module.exports = porcupineKeywords;
})();`,
);

const modelDir = path.join(rootDir, "lib", "common");

outputDirectory = path.join(__dirname, "..", "models");
if (fs.existsSync(outputDirectory)) {
  fs.readdirSync(outputDirectory).forEach((k) => {
    fs.unlinkSync(path.join(outputDirectory, k));
  });
} else {
  fs.mkdirSync(outputDirectory, { recursive: true });
}

const modelName = `porcupine_params${suffix}.pv`;
fs.copyFileSync(
  path.join(modelDir, modelName),
  path.join(outputDirectory, modelName),
);

fs.writeFileSync(
  path.join(outputDirectory, "porcupineModel.js"),
  `const porcupineModel = {
  publicPath: "models/${modelName}",  
  customWritePath: "${version}_${modelName}",
};

(function () {
  if (typeof module !== "undefined" && typeof module.exports !== "undefined")
    module.exports = porcupineModel;
})();`,
);

const command = process.platform === "win32" ? "npx.cmd" : "npx";

child_process.execSync(`${command} http-server -a localhost -p 5000`, {
  shell: true,
  stdio: "inherit",
});
