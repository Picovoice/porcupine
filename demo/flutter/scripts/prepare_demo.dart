import "package:path/path.dart";

import "dart:convert";
import "dart:io";

final String resourcePath = join(dirname(Platform.script.path), "..", "..", "..", "resources");
final String libPath = join(dirname(Platform.script.path), "..", "..", "..", "lib");
final String testDataPath = join(resourcePath, ".test", "test_data.json");

final String assetsPath = join(dirname(Platform.script.path), "..", "assets");
final String keywordsPath = join(assetsPath, "keywords");
final String modelsPath = join(assetsPath, "models");

Future<Map> readJsonFile(String filePath) async {
  var input = await File(filePath).readAsString();
  var map = jsonDecode(input);
  return map;
}

void main(List<String> arguments) async {
  var testData = await readJsonFile(testDataPath);
  List<String> availableLanguages = List<String>.from(testData["tests"]["singleKeyword"].map((x) => x["language"]).toList());

  if (arguments.isEmpty) {
    print("Choose the language you would like to run the demo in with 'dart start [language]'.\n"
        "Available languages are ${availableLanguages.join(", ")}.");
    exit(1);
  }

  String language = arguments[0];
  String suffix = (language == "en") ? "" : "_$language";
  if (!availableLanguages.contains(language)) {
    print("'$language' is not an available demo language.\n"
        "Available languages are ${availableLanguages.join(", ")}.");
    exit(1);
  }

  var assetsDir = Directory(assetsPath);
  if (assetsDir.existsSync()) {
    assetsDir.deleteSync(recursive: true);
  }
  assetsDir.createSync();

  var androidKeywordDir = Directory(join(resourcePath, "keyword_files$suffix", "android"));
  var iOSKeywordDir = Directory(join(resourcePath, "keyword_files$suffix", "ios"));

  var keywordDir = Directory(keywordsPath);
  if (!keywordDir.existsSync()) {
    keywordDir.createSync(recursive: true);
  }

  var modelDir = Directory(modelsPath);
  if (!modelDir.existsSync()) {
    modelDir.createSync(recursive: true);
  }

  if (language != "en") {
    androidKeywordDir.listSync().forEach((k) {
      File f = File(k.path);
      f.copySync(join(keywordDir.path, basename(k.path)));
    });

    iOSKeywordDir.listSync().forEach((k) {
      File f = File(k.path);
      f.copySync(join(keywordDir.path, basename(k.path)));
    });

    File model = File(join(libPath, "common", "porcupine_params$suffix.pv"));
    model.copySync(join(modelDir.path, basename(model.path)));
  }

}