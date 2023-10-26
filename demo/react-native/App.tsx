//
// Copyright 2020-2023 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import React, {Component} from 'react';
import {Platform, StyleSheet, Text, TouchableOpacity, View} from 'react-native';
import {
  PorcupineManager,
  BuiltInKeywords,
  PorcupineErrors,
} from '@picovoice/porcupine-react-native';
import {Picker} from '@react-native-picker/picker';

import {language, keywords} from './params.json';

type Props = {};
type State = {
  currentKeyword: string | BuiltInKeywords;
  keywordOptions: (string | BuiltInKeywords)[];
  buttonText: string;
  buttonDisabled: boolean;
  isListening: boolean;
  backgroundColour: string;
  isError: boolean;
  errorMessage: string | null;
};

export default class App extends Component<Props, State> {
  _porcupineManager: PorcupineManager | undefined;
  _detectionColour: string = '#00E5C3';
  _defaultColour: string = '#F5FCFF';
  _accessKey: string = '${YOUR_ACCESS_KEY_HERE}'; // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)

  constructor(props: Props) {
    super(props);

    const keywordOptions =
      language === 'en' ? Object.values(BuiltInKeywords) : keywords;

    this.state = {
      keywordOptions: keywordOptions,
      currentKeyword: keywordOptions[0],
      buttonText: 'Start',
      buttonDisabled: false,
      isListening: false,
      backgroundColour: this._defaultColour,
      isError: false,
      errorMessage: null,
    };
  }

  async componentDidMount() {
    await this._loadNewKeyword(this.state.currentKeyword);
  }

  async componentWillUnmount() {
    if (this.state.isListening) {
      await this._stopProcessing();
    }
    this._porcupineManager?.delete();
  }

  async _startProcessing() {
    this.setState({
      buttonDisabled: true,
    });

    try {
      await this._porcupineManager?.start();
      this.setState({
        buttonText: 'Stop',
        buttonDisabled: false,
        isListening: true,
      });
    } catch (e: any) {
      this.setState({
        isError: true,
        errorMessage: e.message,
      });
    }
  }

  async _stopProcessing() {
    this.setState({
      buttonDisabled: true,
    });

    try {
      await this._porcupineManager?.stop();
      this.setState({
        buttonText: 'Start',
        buttonDisabled: false,
        isListening: false,
      });
    } catch (e: any) {
      this.setState({
        isError: true,
        errorMessage: e.message,
      });
    }
  }

  async _toggleListening() {
    if (this.state.isListening) {
      await this._stopProcessing();
    } else {
      await this._startProcessing();
    }
  }

  async _loadNewKeyword(keyword: string | BuiltInKeywords) {
    if (this.state.isListening) {
      await this._stopProcessing();
    }
    this._porcupineManager?.delete();

    this.setState({currentKeyword: keyword});

    const detectionCallback = (keywordIndex: number) => {
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
    };

    const processErrorCallback = (error: PorcupineErrors.PorcupineError) => {
      this._stopProcessing();
      this.setState({
        isError: true,
        errorMessage: error.message,
      });
    };

    try {
      if (language === 'en') {
        this._porcupineManager = await PorcupineManager.fromBuiltInKeywords(
          this._accessKey,
          [keyword as BuiltInKeywords],
          detectionCallback,
          processErrorCallback,
        );
      } else {
        const keywordPath = `keywords/${keyword}_${Platform.OS}.ppn`;
        const modelPath = `models/porcupine_params_${language}.pv`;

        this._porcupineManager = await PorcupineManager.fromKeywordPaths(
          this._accessKey,
          [keywordPath],
          detectionCallback,
          processErrorCallback,
          modelPath,
        );
      }
    } catch (err: any) {
      let errorMessage: string;
      if (err instanceof PorcupineErrors.PorcupineInvalidArgumentError) {
        errorMessage = `${err.message}`;
      } else if (err instanceof PorcupineErrors.PorcupineActivationError) {
        errorMessage = 'AccessKey activation error';
      } else if (err instanceof PorcupineErrors.PorcupineActivationLimitError) {
        errorMessage = 'AccessKey reached its device limit';
      } else if (
        err instanceof PorcupineErrors.PorcupineActivationRefusedError
      ) {
        errorMessage = 'AccessKey refused';
      } else if (
        err instanceof PorcupineErrors.PorcupineActivationThrottledError
      ) {
        errorMessage = 'AccessKey has been throttled';
      } else {
        errorMessage = err.toString();
      }

      this.setState({
        isError: true,
        errorMessage: errorMessage,
      });
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
          {backgroundColor: this.state.backgroundColour},
        ]}>
        <View style={styles.statusBar}>
          <Text style={styles.statusBarText}>Porcupine</Text>
        </View>
        <View style={styles.keyword}>
          <Text style={styles.keywordText}>Keyword</Text>
          <View style={styles.picker}>
            <Picker
              selectedValue={this.state.currentKeyword}
              mode="dropdown"
              style={{}}
              itemStyle={styles.itemStyle}
              onValueChange={(keyword) => this._loadNewKeyword(keyword)}>
              {keywordOptions}
            </Picker>
          </View>
        </View>

        <View style={styles.button}>
          <TouchableOpacity
            style={styles.buttonStyle}
            onPress={() => this._toggleListening()}
            disabled={this.state.buttonDisabled || this.state.isError}>
            <Text style={styles.buttonText}>{this.state.buttonText}</Text>
          </TouchableOpacity>
        </View>
        {this.state.isError && (
          <View style={styles.errorBox}>
            <Text style={styles.errorText}>{this.state.errorMessage}</Text>
          </View>
        )}
        <View style={styles.footerView}>
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
    flex: 0.4,
    backgroundColor: '#377DFF',
    justifyContent: 'flex-end',
  },
  statusBarText: {
    fontSize: 18,
    color: 'white',
    fontWeight: 'bold',
    marginLeft: 15,
    marginBottom: 15,
  },
  keyword: {
    flex: 1,
    paddingTop: '10%',
  },
  keywordText: {
    color: '#666666',
    marginLeft: 15,
    marginBottom: 5,
  },
  picker: {
    width: '90%',
    height: '40%',
    alignContent: 'center',
    justifyContent: 'center',
    alignSelf: 'center',
  },
  button: {
    flex: 1,
    justifyContent: 'center',
    alignContent: 'center',
  },
  buttonStyle: {
    width: '50%',
    height: '50%',
    alignSelf: 'center',
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
  footerView: {
    flex: 1,
    justifyContent: 'flex-end',
    paddingBottom: 25,
  },
  instructions: {
    textAlign: 'center',
    color: '#666666',
  },
  errorBox: {
    backgroundColor: 'red',
    borderRadius: 5,
    margin: 20,
    padding: 20,
    textAlign: 'center',
  },
  errorText: {
    color: 'white',
    fontSize: 16,
  },
});
