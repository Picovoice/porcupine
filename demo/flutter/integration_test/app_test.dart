import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';

import 'package:flutter/services.dart' show rootBundle;
import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';

import 'package:porcupine_flutter/porcupine.dart';
import 'package:porcupine_flutter/porcupine_error.dart';

void main() {
  IntegrationTestWidgetsFlutterBinding.ensureInitialized();

  final String accessKey = "{TESTING_ACCESS_KEY_HERE}";
  final String platform = Platform.isAndroid
      ? "android"
      : Platform.isIOS
          ? "ios"
          : throw ("Unsupported platform");

  Future<List<int>> loadAudioFile(String audioPath) async {
    List<int> pcm = [];
    var audioFileData = await rootBundle.load(audioPath);
    for (int i = 44; i < audioFileData.lengthInBytes; i += 2) {
      pcm.add(audioFileData.getInt16(i, Endian.little));
    }
    return pcm;
  }

  group('Porcupine Keyword Tests', () {
    late dynamic testData;

    setUp(() async {
      String testDataJson =
          await rootBundle.loadString('assets/test_resources/test_data.json');
      testData = json.decode(testDataJson);
    });

    testWidgets('Test singleKeyword all languages', (tester) async {
      for (int t = 0; t < testData['tests']['singleKeyword'].length; t++) {
        String language = testData['tests']['singleKeyword'][t]['language'];
        String keyword = testData['tests']['singleKeyword'][t]['wakeword'];

        String keywordPath =
            "assets/test_resources/keyword_files/${keyword}_${platform}.ppn";
        String modelPath =
            "assets/test_resources/model_files/porcupine_params${language != "en" ? "_${language}" : ""}.pv";

        Porcupine porcupine;
        try {
          porcupine = await Porcupine.fromKeywordPaths(accessKey, [keywordPath],
              modelPath: modelPath);
        } on PorcupineException catch (ex) {
          expect(ex, equals(null),
              reason: "Failed to initialize Porcupine for ${language}: ${ex}");
          return;
        }

        String audioPath =
            "assets/test_resources/audio_samples/${keyword.replaceAll(' ', '_')}.wav";
        List<int> pcm = await loadAudioFile(audioPath);

        List<int> detections = [];
        final int frameLength = porcupine.frameLength;
        for (int i = 0; i < (pcm.length - frameLength); i += frameLength) {
          int detection =
              await porcupine.process(pcm.sublist(i, i + frameLength));
          if (detection != -1) {
            detections.add(detection);
          }
        }

        porcupine.delete();
        expect(detections.length, equals(1),
            reason:
                "Number of detections for ${language} ${keyword} was incorrect");
        expect(detections[0], equals(0),
            reason:
                "Porcupine returned wrong keyword index for ${language} ${keyword}");
      }
    });

    testWidgets('Test multipleKeyword all languages', (tester) async {
      for (int t = 0; t < testData['tests']['multipleKeyword'].length; t++) {
        String language = testData['tests']['multipleKeyword'][t]['language'];
        List<String> keywords = List<String>.from(
            testData['tests']['multipleKeyword'][t]['wakewords']);
        List<int> groundTruth = List<int>.from(
            testData['tests']['multipleKeyword'][t]['groundTruth']);

        List<String> keywordPaths = keywords.map((keyword) {
          return "assets/test_resources/keyword_files/${keyword}_${platform}.ppn";
        }).toList();
        String modelPath =
            "assets/test_resources/model_files/porcupine_params${language != "en" ? "_${language}" : ""}.pv";

        Porcupine porcupine;
        try {
          porcupine = await Porcupine.fromKeywordPaths(accessKey, keywordPaths,
              modelPath: modelPath);
        } on PorcupineException catch (ex) {
          expect(ex, equals(null),
              reason: "Failed to initialize Porcupine for ${language}: ${ex}");
          return;
        }

        String audioPath =
            "assets/test_resources/audio_samples/multiple_keywords${language != "en" ? "_${language}" : ""}.wav";
        List<int> pcm = await loadAudioFile(audioPath);

        List<int> detections = [];
        final int frameLength = porcupine.frameLength;
        for (int i = 0; i < (pcm.length - frameLength); i += frameLength) {
          int detection =
              await porcupine.process(pcm.sublist(i, i + frameLength));
          if (detection != -1) {
            detections.add(detection);
          }
        }

        porcupine.delete();
        expect(detections.length, equals(groundTruth.length),
            reason: "Number of detections for ${language} was incorrect");
        expect(detections, equals(groundTruth),
            reason: "Porcupine detections don't match truth for ${language}");
      }
    });
  });
}
