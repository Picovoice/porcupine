import React, { Component } from 'react';
import { PermissionsAndroid, Platform, TouchableOpacity } from 'react-native';
import { StyleSheet, Text, View } from 'react-native';
import { PorcupineManager } from 'react-native-porcupine';
import { Picker } from '@react-native-picker/picker';

type Props = {};
type State = {
  currentKeyword: string;
  keywordOptions: string[];
  buttonText: string;
  isListening: boolean;
  backgroundColour: string;

  error: string | null;
};

export default class App extends Component<Props, State> {
  _porcupineManager: PorcupineManager | undefined;
  _detectionColour: string = '#00E5C3';
  _defaultColour: string = '#F5FCFF';

  constructor(props: Props) {
    super(props);
    let keywordOptions = PorcupineManager.KEYWORDS;
    this.state = {
      keywordOptions: keywordOptions,
      currentKeyword: keywordOptions[0],
      buttonText: 'Start',
      isListening: false,
      backgroundColour: this._defaultColour,

      error: null,
    };
  }

  async componentDidMount() {
    this._loadNewKeyword(this.state.currentKeyword as string);
  }

  componentWillUnmount() {
    if (this.state.isListening) {
      this._stopProcessing();
    }
    this._porcupineManager?.delete();
  }

  async _startProcessing() {
    let recordAudioRequest;
    if (Platform.OS == 'android') {
      recordAudioRequest = this._requestRecordAudioPermission();
    } else {
      recordAudioRequest = new Promise(function (resolve, _) {
        resolve(true);
      });
    }

    recordAudioRequest.then((hasPermission) => {
      if (!hasPermission) {
        return;
      }

      this._porcupineManager?.start();
      this.setState({
        buttonText: 'Stop',
        isListening: true,
      });
    });
  }

  _stopProcessing() {
    this._porcupineManager?.stop();
    this.setState({
      buttonText: 'Start',
      isListening: false,
    });
  }

  _toggleListening() {
    if (this.state.isListening) this._stopProcessing();
    else this._startProcessing();
  }

  async _loadNewKeyword(keyword: string) {
    if (this.state.isListening) {
      this._stopProcessing();
    }
    this._porcupineManager?.delete();

    this.setState({ currentKeyword: keyword });
    try {
      this._porcupineManager = await PorcupineManager.fromKeywords(
        [keyword],
        (keywordIndex: number) => {
          console.log('[wake word detected]');
          if (keywordIndex >= 0) {
            this.setState({
              backgroundColour: this._detectionColour,
            });

            setTimeout(() => {
              this.setState({
                backgroundColour: this._defaultColour,
              });
            }, 1000);
          }
        }
      );
    } catch (err) {
      console.error(err);
    }
  }

  async _requestRecordAudioPermission() {
    try {
      const granted = await PermissionsAndroid.request(
        PermissionsAndroid.PERMISSIONS.RECORD_AUDIO,
        {
          title: 'Microphone Permission',
          message:
            'Porcupine needs access to your microphone to listen for wake words.',
          buttonNeutral: 'Ask Me Later',
          buttonNegative: 'Cancel',
          buttonPositive: 'OK',
        }
      );
      if (granted === PermissionsAndroid.RESULTS.GRANTED) {
        return true;
      } else {
        return false;
      }
    } catch (err) {
      console.error(err);
      return false;
    }
  }

  render() {
    let keywordOptions = this.state.keywordOptions.map((s, i) => {
      return <Picker.Item key={i} value={s} label={s} />;
    });

    return (
      <View
        style={[
          styles.container,
          { backgroundColor: this.state.backgroundColour },
        ]}
      >
        <View style={styles.statusBar}>
          <Text style={styles.statusBarText}>Porcupine</Text>
        </View>
        <View style={styles.subContainer}>
          <Text style={{ color: '#666666', marginLeft: 15, marginBottom: 5 }}>
            Keyword
          </Text>
          <Picker
            selectedValue={this.state.currentKeyword}
            mode="dropdown"
            style={{ height: 60, width: 300, marginHorizontal: 30 }}
            itemStyle={styles.itemStyle}
            onValueChange={(keyword) => this._loadNewKeyword(keyword as string)}
          >
            {keywordOptions}
          </Picker>
        </View>

        <View style={[styles.subContainer, { alignItems: 'center' }]}>
          <TouchableOpacity
            style={styles.buttonStyle}
            onPress={() => this._toggleListening()}
          >
            <Text style={styles.buttonText}>{this.state.buttonText}</Text>
          </TouchableOpacity>
        </View>
        <View style={{ flex: 1, justifyContent: 'flex-end', marginBottom: 10 }}>
          <Text style={styles.instructions}>
            Made in Vancouver, Canada by Picovoice
          </Text>
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: 'column',
    justifyContent: 'center',
    backgroundColor: '#F5FCFF',
  },
  subContainer: {
    flex: 1,
    justifyContent: 'center',
  },
  statusBar: {
    flex: 0.25,
    backgroundColor: '#377DFF',
    justifyContent: 'center',
  },
  statusBarText: {
    fontSize: 18,
    color: 'white',
    fontWeight: 'bold',
    marginLeft: 15,
  },
  buttonStyle: {
    height: 120,
    width: 120,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: '#377DFF',
    borderRadius: 100,
  },
  buttonText: {
    fontSize: 30,
    fontWeight: 'bold',
    color: 'white',
    textAlign: 'center',
  },
  itemStyle: {
    fontWeight: 'bold',
    fontSize: 20,
    textAlign: 'center',
  },
  instructions: {
    textAlign: 'center',
    color: '#666666',
    marginBottom: 5,
  },
});
