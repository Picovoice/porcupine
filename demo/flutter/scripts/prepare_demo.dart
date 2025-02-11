import "package:path/path.dart";

import "dart:convert";
import "dart:io";

final String resourcePath =
    join(dirname(Platform.script.toFilePath()), "..", "..", "..", "resources");
final String libPath =
    join(dirname(Platform.script.toFilePath()), "..", "..", "..", "lib");
final String testDataPath = join(resourcePath, ".test", "test_data.json");

final String assetsPath = join(dirname(Platform.script.toFilePath()), "..", "assets");
final String keywordsPath = join(assetsPath, "keywords");
final String modelsPath = join(assetsPath, "models");

Future<Map> readJsonFile(String filePath) async {
  var input = await File(filePath).readAsString();
  var map = jsonDecode(input);
  return map;
}

void main(List<String> arguments) async {
  var testData = await readJsonFile(testDataPath);
  List<String> availableLanguages = List<String>.from(
      testData["tests"]["singleKeyword"].map((x) => x["language"]).toList());

  if (arguments.isEmpty) {
    print(
        "Choose the language you would like to run the demo in with 'dart scripts/prepare_demo.dart [language]'.\n"
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

  var androidResourceDir =
      Directory(join(resourcePath, "keyword_files$suffix", "android"));
  var iOSResourceDir =
      Directory(join(resourcePath, "keyword_files$suffix", "ios"));

  var keywordAndroidDir = Directory(join(keywordsPath, 'android'));
  if (keywordAndroidDir.existsSync()) {
    keywordAndroidDir.deleteSync(recursive: true);
  }
  keywordAndroidDir.createSync(recursive: true);

  var keywordIosDir = Directory(join(keywordsPath, 'ios'));
  if (keywordIosDir.existsSync()) {
    keywordIosDir.deleteSync(recursive: true);
  }
  keywordIosDir.createSync(recursive: true);

  var modelDir = Directory(modelsPath);
  if (modelDir.existsSync()) {
    modelDir.deleteSync(recursive: true);
  }
  modelDir.createSync(recursive: true);

  var params = {};
  params["language"] = language;
  params["keywords"] = [];

  if (language != "en") {
    androidResourceDir.listSync().forEach((k) {
      File f = File(k.path);
      f.copySync(join(keywordAndroidDir.path, basename(k.path)));
      params["keywords"].add(basename(k.path).replaceAll("_android.ppn", ""));
    });

    iOSResourceDir.listSync().forEach((k) {
      File f = File(k.path);
      f.copySync(join(keywordIosDir.path, basename(k.path)));
    });

    File model = File(join(libPath, "common", "porcupine_params$suffix.pv"));
    model.copySync(join(modelDir.path, basename(model.path)));
  }

  var encoded = json.encode(params);
  File f = File(join(assetsPath, "params.json"));
  f.writeAsStringSync(encoded);

  print("Demo is ready to run!");
}
