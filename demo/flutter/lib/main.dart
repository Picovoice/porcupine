import 'package:flutter/material.dart';
import 'package:flutter_voice_processor/flutter_voice_processor.dart';
import 'dart:async';

import 'package:porcupine/porcupine.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';
  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    try {
      Porcupine p = await Porcupine.fromKeywords(["porcupine"]);
      VoiceProcessor v =
          VoiceProcessor.getVoiceProcessor(p.frameLength, p.sampleRate);

      v.addListener((buffer) {
        List<int> bufferData = (buffer as List<dynamic>).cast<int>();
        int keywordIdx = p.process(bufferData);
        if (keywordIdx == 0) {
          print("[wake word detected]");
        }
      });
      if (await v.hasRecordAudioPermission()) {
        await v.start();
      }
      // p.delete();
    } on Exception {
      print("oops");
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Text('Running on: $_platformVersion\n'),
        ),
      ),
    );
  }
}
