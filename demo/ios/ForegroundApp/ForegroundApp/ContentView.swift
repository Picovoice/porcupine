//
//  Copyright 2025 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import SwiftUI

struct ContentView: View {
    @StateObject private var viewModel = ViewModel()

    var body: some View {
        ZStack {
            viewModel.backgroundColor.ignoresSafeArea()

            VStack {
                Picker("Wake Word", selection: $viewModel.selectedWakeWord) {
                    ForEach(viewModel.wakeWordKeys, id: \.self) { word in
                        Text(word.capitalized).tag(word)
                    }
                }
                .pickerStyle(WheelPickerStyle())
                .disabled(viewModel.isListening)

                if let message = viewModel.errorMessage,
                   viewModel.errorMessage != nil {
                    Text(message)
                        .foregroundColor(.white)
                        .padding()
                        .background(Color.red)
                        .cornerRadius(10)
                        .padding(.horizontal)
                }

                Button(action: viewModel.toggleListening) {
                    Text(viewModel.isListening ? "STOP" : "START")
                        .font(.system(size: 30, weight: .bold))
                        .frame(width: 120, height: 120)
                        .background(
                            viewModel.errorMessage != nil
                             ? Color.gray
                             : Color(red: 55.0/255, green: 125.0/255, blue: 255.0/255)
                        )
                        .foregroundColor(.white)
                        .clipShape(Circle())
                }
                .padding(.top, 30)
                .disabled(viewModel.errorMessage != nil)
            }
        }
        .onAppear(perform: viewModel.setupWakeWords)
    }
}

struct Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
