//
// Copyright 2020-2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import 'dart:async';
import 'dart:io';
import 'dart:typed_data';

import 'package:flutter/services.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'package:path_provider/path_provider.dart';
import 'package:flutter_porcupine/porcupine_error.dart';

enum BuiltInKeyword {
  ALEXA,
  AMERICANO,
  BLUEBERRY,
  BUMBLEBEE,
  COMPUTER,
  GRAPEFRUIT,
  GRASSHOPPER,
  HEY_GOOGLE,
  HEY_SIRI,
  JARVIS,
  OK_GOOGLE,
  PICOVOICE,
  PORCUPINE,
  TERMINATOR
}

class Porcupine {
  static final MethodChannel _channel = MethodChannel("porcupine");

  static String? _defaultModelPath;
  static Map<BuiltInKeyword, String>? _builtInKeywordPaths;

  String? _handle;
  final int _frameLength;
  final int _sampleRate;
  final String _version;

  /// Porcupine version string
  String get version => _version;

  /// The number of audio samples per frame required by Porcupine
  int get frameLength => _frameLength;

  /// The audio sample rate required by Porcupine
  int get sampleRate => _sampleRate;

  /// Static creator for initializing Porcupine from a selection of built-in keywords
  ///
  /// [accessKey] AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
  ///
  /// [keywords] is a List of (phrases) for detection. The list of available
  /// keywords can be retrieved using [BuiltInKeyword] enum.
  ///
  /// [modelPath] is a path to the file containing model parameters. If not set
  /// it will be set to the default location.
  ///
  /// [sensitivities] sensitivities for each keywords model. A higher sensitivity
  /// reduces miss rate at the cost of potentially higher false alarm rate.
  /// Sensitivity should be a floating-point number within 0 and 1.
  ///
  /// Thows a `PvError` if not initialized correctly
  ///
  /// returns an instance of the wake word engine
  static Future<Porcupine> fromBuiltInKeywords(
      String accessKey, List<BuiltInKeyword> keywords,
      {String? modelPath, List<double>? sensitivities}) async {

    await _init();

    List<String?> keywordPaths = List.empty(growable: true);
    for (var builtIn in keywords) {
      keywordPaths.add(_builtInKeywordPaths?[builtIn]);
    }

    return Porcupine._create(
        accessKey: accessKey,
        keywordPaths: keywordPaths,
        modelPath: modelPath,
        sensitivities: sensitivities);
  }

  /// Static creator for initializing Porcupine from a list of paths to custom keyword files
  ///
  /// [accessKey] AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
  ///
  /// [keywordPaths] A List of absolute paths to keyword model files.
  ///
  /// [modelPath] is a path to the file containing model parameters. If not set
  /// it will be set to the default location.
  ///
  /// [sensitivities] sensitivities for each keywords model. A higher sensitivity
  /// reduces miss rate at the cost of potentially higher false alarm rate.
  /// Sensitivity should be a floating-point number within 0 and 1.
  ///
  /// Thows a `PvError` if not initialized correctly
  ///
  /// returns an instance of the wake word engine
  static Future<Porcupine> fromKeywordPaths(
      String accessKey, List<String?>? keywordPaths,
      {String? modelPath, List<double>? sensitivities}) async {
        
    await _init();

    return Porcupine._create(
        accessKey: accessKey,
        keywordPaths: keywordPaths,
        modelPath: modelPath,
        sensitivities: sensitivities);
  }

  /// private static creator
  static Future<Porcupine> _create(
      {String? accessKey,
      List<String?>? keywordPaths,
      String? modelPath,
      List<double>? sensitivities}) async {
    if (accessKey == null || accessKey == "") {
      throw PorcupineInvalidArgumentException("'accessKey' must be set.");
    }

    if (modelPath == null || modelPath == "") {
      modelPath = _defaultModelPath;
    }

    if (!await File(modelPath!).exists()) {
      try {
        modelPath = await _extractResource(modelPath);
      } catch (_) {
        throw PorcupineInvalidArgumentException(
            "Could not find model file at path '$modelPath'. If this is a packaged asset, ensure you have added it to your pubspec.yaml file.");
      }
    }

    if (keywordPaths == null || keywordPaths.isEmpty) {
      throw PorcupineInvalidArgumentException("'keywordPaths' must be set.");
    }

    for (var i = 0; i < keywordPaths.length; i++) {
      if (keywordPaths[i] == null || keywordPaths[i] == "") {
        throw PorcupineInvalidArgumentException(
            "One of the provided keyword paths was empty.");
      }

      if (!await File(keywordPaths[i]!).exists()) {
        try {
          keywordPaths[i] = await _extractResource(keywordPaths[i]!);
        } catch (_) {
          throw PorcupineInvalidArgumentException(
              "Could not find keyword file at path '${keywordPaths[i]}'. If this is a packaged asset, ensure you have added it to your pubspec.yaml file.");
        }
      }
    }

    if (sensitivities == null || sensitivities.isEmpty) {
      sensitivities = List.filled(keywordPaths.length, 0.5);
    } else {
      for (double sensitivity in sensitivities) {
        if (sensitivity < 0 || sensitivity > 1 || sensitivity == double.nan) {
          throw PorcupineInvalidArgumentException(
              "Sensitivity value ($sensitivity) in given 'sensitivities' not in range [0,1]");
        }
      }

      if (keywordPaths.length != sensitivities.length) {
        throw PorcupineInvalidArgumentException(
            "Number of keywords (${keywordPaths.length}) does not match number of sensitivities (${sensitivities.length})");
      }
    }

    try {
      Map<String, dynamic> result =
          Map<String, dynamic>.from(await _channel.invokeMethod('create', {
        'accessKey': accessKey,
        'modelPath': modelPath,
        'keywordPaths': keywordPaths,
        'sensitivities': sensitivities
      }));
      print("res");
      print(result);

      return Porcupine._(result['handle'], result['frameLength'],
          result['sampleRate'], result['version']);
    } on PlatformException catch (error) {
      throw porcupineStatusToException(error.code, error.message);
    } on Exception catch (error) {
      throw porcupineStatusToException("PorcupineException", error.toString());
    }
  }

  static Future<void> _init() async {
    if (_defaultModelPath != null) {
      return;
    }

    try {
      Map<String, dynamic> result =
          Map<String, dynamic>.from(await _channel.invokeMethod('init'));

      _defaultModelPath = result['DEFAULT_MODEL_PATH'];

      Map<String, String> keywordMap =
          Map<String, String>.from(result['KEYWORD_PATHS']);

      for (var builtIn in BuiltInKeyword.values) {
        String keyword = builtIn.toString().toLowerCase().replaceAll('_', ' ');
        _builtInKeywordPaths?[builtIn] = keywordMap[keyword]!;
      }
    } on Exception catch (error) {
      throw porcupineStatusToException("PorcupineException", error.toString());
    }
  }

  // private constructor
  Porcupine._(this._handle, this._frameLength, this._sampleRate, this._version);

  /// Process a frame of audio with the wake word engine.
  ///
  /// [frame] is a frame of frame of audio samples to be assessed by Porcupine. The required audio format is found by
  /// calling `.sampleRate` to get the required sample rate and `.frameLength` to get the required frame size.
  /// Audio must be single-channel and 16-bit linearly-encoded.
  ///
  /// Index of the detected keyword, or -1 if no detection occurred
  Future<int> process(List<int>? frame) async {
    if (_handle == null) {
      throw PorcupineInvalidStateException(
          "Attempted to process an audio frame after Porcupine was been deleted.");
    }

    if (frame == null) {
      throw PorcupineInvalidArgumentException(
          "Frame array provided to Porcupine process was null.");
    }

    if (frame.length != frameLength) {
      throw PorcupineInvalidArgumentException(
          "Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' ($frameLength)");
    }

    int keywordIndex = await _channel
        .invokeMethod('process', {'handle': _handle, 'frame': frame});

    return keywordIndex;
  }

  /// Frees memory that was allocated for Porcupine
  Future<void> delete() async {
    if (_handle != null) {
      await _channel.invokeMethod('delete', {'handle': _handle});
      _handle = null;
    }
  }

  static final String _platform = Platform.isAndroid ? "android" : "ios";
  static const String _assetDir = "packages/porcupine/assets";
  static final String _keywordAssetDir =
      "$_assetDir/resources/keyword_files/$_platform";

  static String _getResourcePath(BuiltInKeyword builtIn) {
    String keyword =
        builtIn.toString().split(".").last.replaceAll("_", " ").toLowerCase();
    return "$_keywordAssetDir/${keyword}_$_platform.ppn";
  }

  static Future<void> _extractPorcupineResources() async {
    _defaultModelPath =
        await _extractResource("$_assetDir/lib/common/porcupine_params.pv");

    _builtInKeywordPaths = {};
    for (var builtIn in BuiltInKeyword.values) {
      String keywordAssetPath = _getResourcePath(builtIn);
      String extractedPath = await _extractResource(keywordAssetPath);

      _builtInKeywordPaths?[builtIn] = extractedPath;
    }
  }

  static Future<String> _extractResource(String filePath) async {
    String resourceDirectory = (await getApplicationDocumentsDirectory()).path;
    String outputPath = '$resourceDirectory/$filePath';
    File outputFile = File(outputPath);

    ByteData data = await rootBundle.load(filePath);
    final buffer = data.buffer;

    await outputFile.create(recursive: true);
    await outputFile.writeAsBytes(
        buffer.asUint8List(data.offsetInBytes, data.lengthInBytes));
    return outputPath;
  }
}
