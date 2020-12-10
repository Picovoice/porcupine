import 'package:flutter/material.dart';
import 'dart:async';

import 'package:porcupine/porcupine_manager.dart';
import 'package:porcupine/porcupine_error.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  bool _isButtonDisabled = false;
  bool _isProcessing = false;
  PorcupineManager _porcupineManager;
  List<String> _keywords = ["porcupine", "alexa", "computer", "hey google"];
  @override
  void initState() {
    super.initState();
    initPorcupineManager();
  }

  Future<void> initPorcupineManager() async {
    try {
      _porcupineManager = await PorcupineManager.fromKeywords(
          _keywords,
          (keywordIndex) =>
              print("${DateTime.now()}: ${_keywords[keywordIndex]} detected"));
    } on PvError catch (ex) {
      print("Failed to initialize Porcupine: ${ex.message}");
    }
  }

  Future<void> _startProcessing() async {
    this.setState(() {
      _isButtonDisabled = true;
    });

    try {
      await _porcupineManager.start();
      this.setState(() {
        _isProcessing = true;
      });
    } on PvAudioException catch (ex) {
      print("Failed to start audio capture: " + ex.message);
    } finally {
      this.setState(() {
        _isButtonDisabled = false;
      });
    }
  }

  Future<void> _stopProcessing() async {
    this.setState(() {
      _isButtonDisabled = true;
    });

    await _porcupineManager.stop();

    this.setState(() {
      _isButtonDisabled = false;
      _isProcessing = false;
    });
  }

  void _toggleProcessing() async {
    if (_isProcessing) {
      await _stopProcessing();
    } else {
      await _startProcessing();
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Porcupine Demo'),
        ),
        body: Center(
          child: _buildToggleProcessingButton(),
        ),
      ),
    );
  }

  Widget _buildToggleProcessingButton() {
    return new RaisedButton(
      onPressed: _isButtonDisabled ? null : _toggleProcessing,
      child: Text(_isProcessing ? "Stop" : "Start",
          style: TextStyle(fontSize: 20)),
    );
  }
}
