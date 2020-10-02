/*
    Copyright 2018-2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;

using PorcupineDotNet;
using System.Linq;

namespace PorcupineTest
{
    [TestClass]
    public class MainTest
    {        
        private static readonly string rootDir = Path.GetFullPath($"{Environment.CurrentDirectory}/../../../../../../../");
        private static readonly string modelFilePath = Path.Combine(rootDir, "lib/common/porcupine_params.pv");
        private static readonly string env = GetEnvironmentName();
        private static readonly Dictionary<string, string> keywordFilePaths = new Dictionary<string, string>
        {
            { "americano", Path.Combine(rootDir, $"resources/keyword_files/{env}/americano_{env}.ppn") },
            { "blueberry", Path.Combine(rootDir, $"resources/keyword_files/{env}/blueberry_{env}.ppn")},
            { "bumblebee", Path.Combine(rootDir, $"resources/keyword_files/{env}/bumblebee_{env}.ppn")},
            { "grapefruit", Path.Combine(rootDir, $"resources/keyword_files/{env}/grapefruit_{env}.ppn")},
            { "grasshopper", Path.Combine(rootDir, $"resources/keyword_files/{env}/grasshopper_{env}.ppn")},
            { "picovoice", Path.Combine(rootDir, $"resources/keyword_files/{env}/picovoice_{env}.ppn")},
            { "porcupine", Path.Combine(rootDir, $"resources/keyword_files/{env}/porcupine_{env}.ppn")},
            { "terminator", Path.Combine(rootDir, $"resources/keyword_files/{env}/terminator_{env}.ppn")}
        };

        private static readonly Dictionary<string, float> sensitivities = new Dictionary<string, float>
        {
            { "americano", 0.5f }, { "blueberry", 0.5f }, { "bumblebee", 0.5f }, { "grapefruit", 0.5f },
            { "grasshopper", 0.5f }, { "picovoice", 0.5f }, { "porcupine", 0.5f }, { "terminator", 0.5f }
        };

        [TestMethod]
        public void TestProcess()
        {                     
            Porcupine p = new Porcupine(modelFilePath, new List<string> { keywordFilePaths["porcupine"] }, new List<float>{ sensitivities["porcupine"] } );
            Assert.IsFalse(string.IsNullOrWhiteSpace(p.Version), "Porcupine did not return a valid version number.");
            
            int frameLen = p.FrameLength;
            Assert.IsTrue(frameLen > 0, "Specified frame length was not a valid number.");

            List<short> data = new List<short>();
            using (BinaryReader reader = new BinaryReader(File.Open(Path.Combine(rootDir, "resources/audio_samples/porcupine.wav"), FileMode.Open)))
            {
                reader.ReadBytes(24);
                Assert.AreEqual(reader.ReadInt32(), p.SampleRate, "Specified sample rate did not match test file.");
                reader.ReadBytes(16);
                
                while (reader.BaseStream.Position != reader.BaseStream.Length)
                {
                    data.Add(reader.ReadInt16());
                }
            }

            int framecount = (int)Math.Floor((float)(data.Count / frameLen));
            var results = new List<int>();
            for (int i = 0; i < framecount; i++)
            {
                int start = i * p.FrameLength;
                int count = p.FrameLength;
                List<short> frame = data.GetRange(start, count);
                int result = p.Process(frame.ToArray());
                Assert.IsTrue(result >= -1, "Porcupine returned an unexpected result (<-1)");

                if (result >= 0)
                {
                    results.Add(result);
                }
            }

            Assert.IsTrue(results.Count == 1 && results[0] == 0, "Expected to find keyword 'porcupine', but no keyword was detected.");
            p.Dispose();
        }

        [TestMethod]
        public void TestProcessMultiple()
        {                        
            Porcupine p = new Porcupine(modelFilePath, keywordFilePaths.Values.ToList(), sensitivities.Values.ToList());
            Assert.IsFalse(string.IsNullOrWhiteSpace(p.Version), "Porcupine did not return a valid version number.");

            int frameLen = p.FrameLength;
            Assert.IsTrue(frameLen > 0, "Specified frame length was not a valid number.");

            List<short> data = new List<short>();
            using (BinaryReader reader = new BinaryReader(File.Open(Path.Combine(rootDir, "resources/audio_samples/multiple_keywords.wav"), FileMode.Open)))
            {
                reader.ReadBytes(24);
                Assert.AreEqual(reader.ReadInt32(), p.SampleRate, "Specified sample rate did not match test file.");
                reader.ReadBytes(16);

                while (reader.BaseStream.Position != reader.BaseStream.Length)
                {
                    data.Add(reader.ReadInt16());
                }
            }

            int framecount = (int)Math.Floor((float)(data.Count / frameLen));
            var results = new List<int>();
            for (int i = 0; i < framecount; i++)
            {
                int start = i * p.FrameLength;
                int count = p.FrameLength;
                List<short> frame = data.GetRange(start, count);
                int result = p.Process(frame.ToArray());
                Assert.IsTrue(result >= -1, "Porcupine returned an unexpected result (<-1)");

                if (result >= 0)
                {
                    results.Add(result);
                }
            }

            var expectedResults = new[] { 6, 0, 1, 2, 3, 4, 5, 6, 7 };
            Assert.AreEqual(expectedResults.Length, results.Count, $"Should have found {expectedResults.Length} keywords, but {results.Count} were found.");
            for (int i = 0; i < results.Count; i++)
            {
                Assert.AreEqual(expectedResults[i], results[i], 
                    $"Expected '{keywordFilePaths.ElementAt(expectedResults[i]).Key}', but '{keywordFilePaths.ElementAt(results[i]).Key}' was detected.");
            }

            p.Dispose();
        }

        private static string GetEnvironmentName()
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                return "mac";
            }
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                return "linux";
            }
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return "windows";
            }

            throw new NotImplementedException("this OS has no binding logic implemented yet.");
        }
    }
}
