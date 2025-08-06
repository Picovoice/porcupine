//
// Copyright 2023 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import React, {useState} from 'react';
import {
  Button,
  SafeAreaView,
  ScrollView,
  StatusBar,
  Text,
  useColorScheme,
  View,
} from 'react-native';
import {Colors} from 'react-native/Libraries/NewAppScreen';

import {runPorcupineTests, Result} from './Tests';

function printResults(results: Result[]) {
  return results.map(result => {
    return (
      <View
        key={result.testName}
        style={{
          backgroundColor: result.success ? 'green' : 'red',
          marginBottom: 5,
          padding: 5,
        }}>
        <Text>{result.testName}</Text>
        <Text testID="testResult">{`${result.success}`}</Text>
        {result.errorString ? <Text>{`${result.errorString}`}</Text> : <></>}
      </View>
    );
  });
}

function App(): React.JSX.Element {
  const isDarkMode = useColorScheme() === 'dark';
  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
    flex: 1,
  };
  const margined = {
    marginTop: StatusBar.currentHeight,
  }

  /*
   * To keep the template simple and small we're adding padding to prevent view
   * from rendering under the System UI.
   * For bigger apps the recommendation is to use `react-native-safe-area-context`:
   * https://github.com/AppAndFlow/react-native-safe-area-context
   *
   * You can read more about it here:
   * https://github.com/react-native-community/discussions-and-proposals/discussions/827
   */
  const safePadding = '5%';

  const [running, setRunning] = useState(false);
  const [results, setResults] = useState([]);

  const runTests = async () => {
    setRunning(true);
    const newResults = await runPorcupineTests();
    setResults(newResults);
    setRunning(false);
  };

  return (
    <SafeAreaView style={backgroundStyle}>
      <StatusBar
        barStyle={isDarkMode ? 'light-content' : 'dark-content'}
        backgroundColor={backgroundStyle.backgroundColor}
      />
      <ScrollView
        contentInsetAdjustmentBehavior="automatic"
        style={{...backgroundStyle, ...margined}}>
        <View
          style={{
            backgroundColor: isDarkMode ? Colors.black : Colors.white,
          }}>
          <Button title="Run Tests" testID="runTests" onPress={runTests} />
          {printResults(results)}
          {running ? (
            <Text testID="testStatus">Tests running, please wait...</Text>
          ) : (
            ''
          )}
        </View>
      </ScrollView>
    </SafeAreaView>
  );
}

export default App;
