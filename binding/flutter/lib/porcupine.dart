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
import 'package:porcupine_flutter/porcupine_error.dart';

/// BuiltInKeywords
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
  /// Thows a `PorcupineException` if not initialized correctly
  ///
  /// returns an instance of the wake word engine
  static Future<Porcupine> fromBuiltInKeywords(
      String accessKey, List<BuiltInKeyword> keywords,
      {String? modelPath, List<double>? sensitivities}) async {
    if (modelPath != null) {
      modelPath = await _extractResource(modelPath);
    }

    List<String> keywordValues = List.empty(growable: true);
    for (var keyword in keywords) {
      keywordValues.add(keyword.toString().split('.').last);
    }

    try {
      Map<String, dynamic> result = Map<String, dynamic>.from(
          await _channel.invokeMethod('from_builtin_keywords', {
        'accessKey': accessKey,
        'modelPath': modelPath,
        'keywords': keywordValues,
        'sensitivities': sensitivities
      }));

      return Porcupine._(result['handle'], result['frameLength'],
          result['sampleRate'], result['version']);
    } on PlatformException catch (error) {
      throw porcupineStatusToException(error.code, error.message);
    } on Exception catch (error) {
      throw porcupineStatusToException("PorcupineException", error.toString());
    }
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
  /// Thows a `PorcupineException` if not initialized correctly
  ///
  /// returns an instance of the wake word engine
  static Future<Porcupine> fromKeywordPaths(
      String accessKey, List<String> keywordPaths,
      {String? modelPath, List<double>? sensitivities}) async {
    if (modelPath != null) {
      modelPath = await _extractResource(modelPath);
    }

    for (var i = 0; i < keywordPaths.length; i++) {
      keywordPaths[i] = await _extractResource(keywordPaths[i]);
    }

    try {
      Map<String, dynamic> result = Map<String, dynamic>.from(
          await _channel.invokeMethod('from_keyword_paths', {
        'accessKey': accessKey,
        'modelPath': modelPath,
        'keywordPaths': keywordPaths,
        'sensitivities': sensitivities
      }));

      return Porcupine._(result['handle'], result['frameLength'],
          result['sampleRate'], result['version']);
    } on PlatformException catch (error) {
      throw porcupineStatusToException(error.code, error.message);
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
    try {
      int keywordIndex = await _channel
          .invokeMethod('process', {'handle': _handle, 'frame': frame});

      return keywordIndex;
    } on PlatformException catch (error) {
      throw porcupineStatusToException(error.code, error.message);
    } on Exception catch (error) {
      throw porcupineStatusToException("PorcupineException", error.toString());
    }
  }

  /// Frees memory that was allocated for Porcupine
  Future<void> delete() async {
    if (_handle != null) {
      await _channel.invokeMethod('delete', {'handle': _handle});
      _handle = null;
    }
  }

  static Future<String> _extractResource(String filePath) async {
    ByteData data;
    try {
      data = await rootBundle.load(filePath);
    } catch (_) {
      return filePath;
    }

    try {
      String resourceDirectory =
          (await getApplicationDocumentsDirectory()).path;
      String outputPath = '$resourceDirectory/$filePath';
      File outputFile = File(outputPath);
      final buffer = data.buffer;

      await outputFile.create(recursive: true);
      await outputFile.writeAsBytes(
          buffer.asUint8List(data.offsetInBytes, data.lengthInBytes));
      return outputPath;
    } catch (_) {
      throw porcupineStatusToException(
          "PorcupineIOException", "failed to extract '$filePath'");
    }
  }
}
