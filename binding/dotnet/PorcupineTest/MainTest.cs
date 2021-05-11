/*
    Copyright 2020-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/
                                                                                                               
using System;
using System.IO;
using System.Reflection;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Pv;

namespace PorcupineTest
{
    [TestClass]
    public class MainTest
    {
        [TestMethod]
        public void TestFrameLength() 
        {
            Porcupine p = Porcupine.Create(keywords: new List<string> { "porcupine" });            
            Assert.IsTrue(p.FrameLength > 0, "Specified frame length was not a valid number.");
            p.Dispose();
        }

        [TestMethod]
        public void TestVersion()
        {
            Porcupine p = Porcupine.Create(keywords: new List<string> { "porcupine" });
            Assert.IsFalse(string.IsNullOrWhiteSpace(p.Version), "Porcupine did not return a valid version number.");
            p.Dispose();
        }

        [TestMethod]
        public void TestProcess()
        {                     
            Porcupine p = Porcupine.Create(keywords: new List<string> { "porcupine" });            
            int frameLen = p.FrameLength;
            
            string testAudioPath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "resources/audio_samples/porcupine.wav");
            List<short> data = GetPcmFromFile(testAudioPath, p.SampleRate);

            int framecount = (int)Math.Floor((float)(data.Count / frameLen));
            var results = new List<int>();
            for (int i = 0; i < framecount; i++)
            {
                int start = i * frameLen;
                int count = frameLen;
                List<short> frame = data.GetRange(start, count);
                int result = p.Process(frame.ToArray());
                Assert.IsTrue(result == -1 || result == 0, "Porcupine returned an unexpected result (should return 0 or -1)");
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
            List<string> inputKeywords = new List<string> 
            { 
                "alexa", "americano", "blueberry", 
                "bumblebee", "grapefruit", "grasshopper", 
                "picovoice", "porcupine", "terminator" 
            };
            List<int> expectedResults = new List<int>() { 7, 0, 1, 2, 3, 4, 5, 6, 7, 8 };

            Porcupine p = Porcupine.Create(keywords: inputKeywords);
            int frameLen = p.FrameLength;

            string testAudioPath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "resources/audio_samples/multiple_keywords.wav");
            List<short> data = GetPcmFromFile(testAudioPath, p.SampleRate);

            int framecount = (int)Math.Floor((float)(data.Count / frameLen));
            var results = new List<int>();
            for (int i = 0; i < framecount; i++)
            {
                int start = i * frameLen;
                int count = frameLen;
                List<short> frame = data.GetRange(start, count);
                int result = p.Process(frame.ToArray());
                Assert.IsTrue(result >= -1, "Porcupine returned an unexpected result (<-1)");

                if (result >= 0)
                {
                    results.Add(result);
                }
            }            

            Assert.AreEqual(expectedResults.Count, results.Count, $"Should have found {expectedResults.Count} keywords, but {results.Count} were found.");
            for (int i = 0; i < results.Count; i++)
            {
                Assert.AreEqual(expectedResults[i], results[i], 
                    $"Expected '{Porcupine.KEYWORDS[expectedResults[i]]}', but '{Porcupine.KEYWORDS[results[i]]}' was detected.");
            }

            p.Dispose();
        }

        private List<short> GetPcmFromFile(string audioFilePath, int expectedSampleRate) 
        {
            List<short> data = new List<short>();
            using (BinaryReader reader = new BinaryReader(File.Open(audioFilePath, FileMode.Open)))
            {
                reader.ReadBytes(24); // skip over part of the header
                Assert.AreEqual(reader.ReadInt32(), expectedSampleRate, "Specified sample rate did not match test file.");
                reader.ReadBytes(16); // skip over the rest of the header

                while (reader.BaseStream.Position != reader.BaseStream.Length)
                {
                    data.Add(reader.ReadInt16());
                }
            }

            return data;
        }
    }
}
