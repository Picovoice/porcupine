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
    @StateObject private var viewModel = PorcupineViewModel()

    var body: some View {
        ZStack {
            VStack(spacing: 20) {
                Text("""
                Press the Start button and say the wake word "Porcupine".
                Try pressing the home button and saying it again.
                """)
                    .padding()
                    .frame(maxWidth: .infinity)
                    .background(Color(.systemBackground))
                    .cornerRadius(10)

                Spacer()

                Button(action: {
                    viewModel.toggleListening()
                }, label: {
                    Text(viewModel.isListening ? "STOP" : "START")
                        .font(.headline)
                        .foregroundColor(.white)
                        .frame(width: 100, height: 100)
                        .background(
                            viewModel.errorMessage != nil
                             ? Color.gray
                             : Color(red: 55.0/255, green: 125.0/255, blue: 255.0/255)
                        )
                        .clipShape(Circle())
                })
                .disabled(viewModel.errorMessage != nil)
                .padding(.bottom, 40)
            }
            .padding()

            if viewModel.errorMessage != nil {
                VStack {
                    Text(viewModel.errorMessage ?? "Unknown error")
                        .padding()
                        .frame(maxWidth: .infinity)
                        .background(Color.red.opacity(0.2))
                        .foregroundColor(.red)
                        .cornerRadius(10)
                        .padding()
                }
            }
        }
    }
}

struct PorcupineView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
