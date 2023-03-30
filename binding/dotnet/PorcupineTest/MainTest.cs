/*
    Copyright 2020-2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using Pv;

namespace PorcupineTest
{
    [TestClass]
    public class MainTest
    {
        private static readonly string ROOT_DIR = Path.Combine(
            Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location),
            "../../../../../..");

        private static Architecture _arch => RuntimeInformation.ProcessArchitecture;
        private static string _env => RuntimeInformation.IsOSPlatform(OSPlatform.OSX) ? "mac" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "windows" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Linux) && _arch == Architecture.X64 ? "linux" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Linux) &&
                                                    (_arch == Architecture.Arm || _arch == Architecture.Arm64) ? PvLinuxEnv() : "";

        private static string _accessKey;

        private Porcupine _porcupine;

        private static string PvLinuxEnv()
        {
            string cpuInfo = File.ReadAllText("/proc/cpuinfo");
            string[] cpuPartList = cpuInfo.Split('\n').Where(x => x.Contains("CPU part")).ToArray();
            if (cpuPartList.Length == 0)
            {
                throw new PlatformNotSupportedException($"Unsupported CPU.\n{cpuInfo}");
            }

            string cpuPart = cpuPartList[0].Split(' ').Last().ToLower();

            switch (cpuPart)
            {
                case "0xc07":
                case "0xd03":
                case "0xd08": return "raspberry-pi";
                case "0xd07": return "jetson";
                case "0xc08": return "beaglebone";
                default:
                    throw new PlatformNotSupportedException($"This device (CPU part = {cpuPart}) is not supported by Picovoice.");
            }
        }

        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            _accessKey = Environment.GetEnvironmentVariable("ACCESS_KEY");
        }


        private static JObject LoadJsonTestData()
        {
            string content = File.ReadAllText(Path.Combine(ROOT_DIR, "resources/.test/test_data.json"));
            return JObject.Parse(content);
        }

        [Serializable]
        private class SingleKeywordJson
        {
            public string language { get; set; }
            public string wakeword { get; set; }
        }

        public static IEnumerable<object[]> SingleKeywordTestData
        {
            get
            {
                JObject testDataJson = LoadJsonTestData();
                IList<SingleKeywordJson> singleKeywordJson = ((JArray)testDataJson["tests"]["singleKeyword"]).ToObject<IList<SingleKeywordJson>>();
                return singleKeywordJson
                    .Select(x => new object[] {
                        x.language,
                        x.wakeword,
                        x.wakeword.Replace(' ', '_') + ".wav"
                    });
            }
        }


        [Serializable]
        private class MultipleKeywordJson
        {
            public string language { get; set; }
            public string[] wakewords { get; set; }
            public int[] groundTruth { get; set; }
        }

        public static IEnumerable<object[]> MultipleKeywordTestData
        {
            get
            {
                JObject testDataJson = LoadJsonTestData();
                IList<MultipleKeywordJson> multipleKeywordJson = ((JArray)testDataJson["tests"]["multipleKeyword"]).ToObject<IList<MultipleKeywordJson>>();
                return multipleKeywordJson
                    .Select(x => new object[] {
                        x.language,
                        x.wakewords,
                        AppendLanguage("multiple_keywords", x.language) + ".wav",
                        x.groundTruth
                    });
            }
        }


        private static string AppendLanguage(string s, string language)
        {
            if (language == "en")
            {
                return s;
            }
            return $"{s}_{language}";
        }

        private static string GetKeywordPath(string language, string keyword)
        {
            return Path.Combine(
                ROOT_DIR,
                "resources",
                AppendLanguage("keyword_files", language),
                $"{_env}/{keyword}_{_env}.ppn"
            );
        }

        private static List<string> GetKeywordPaths(string language, List<string> keywords)
        {
            List<string> keywordPaths = new List<string>();
            foreach (string keyword in keywords)
            {
                keywordPaths.Add(GetKeywordPath(language, keyword));
            }
            return keywordPaths;
        }

        private static string GetModelPath(string language)
        {
            string file_name = AppendLanguage("porcupine_params", language);
            return Path.Combine(
                ROOT_DIR,
                "lib/common",
                $"{file_name}.pv"
            );
        }

        private void RunTestCase(string audioFileName, List<int> expectedResults)
        {
            int frameLen = _porcupine.FrameLength;
            string testAudioPath = Path.Combine(ROOT_DIR, "resources/audio_samples", audioFileName);
            List<short> data = GetPcmFromFile(testAudioPath, _porcupine.SampleRate);

            int framecount = (int)Math.Floor((float)(data.Count / frameLen));
            List<int> results = new List<int>();
            for (int i = 0; i < framecount; i++)
            {
                int start = i * frameLen;
                int count = frameLen;
                List<short> frame = data.GetRange(start, count);
                int result = _porcupine.Process(frame.ToArray());
                Assert.IsTrue(result >= -1, "Porcupine returned an unexpected result (<-1)");
                if (result >= 0)
                {
                    results.Add(result);
                }
            }

            Assert.AreEqual(expectedResults.Count, results.Count, $"Should have found {expectedResults.Count} keywords, but {results.Count} were found.");
            for (int i = 0; i < results.Count; i++)
            {
                Assert.AreEqual(expectedResults[i], results[i], $"Expected keyword {expectedResults[i]}, but Porcupine detected keyword {results[i]}.");
            }

            _porcupine.Dispose();
        }

        [TestMethod]
        public void TestFrameLength()
        {
            _porcupine = Porcupine.FromBuiltInKeywords(_accessKey, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            Assert.IsTrue(_porcupine.FrameLength > 0, "Specified frame length was not a valid number.");
            _porcupine.Dispose();
        }

        [TestMethod]
        public void TestVersion()
        {
            _porcupine = Porcupine.FromBuiltInKeywords(_accessKey, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            Assert.IsFalse(string.IsNullOrWhiteSpace(_porcupine.Version), "Porcupine did not return a valid version number.");
            _porcupine.Dispose();
        }

        [TestMethod]
        public void TestSingleKeywordBuiltin()
        {
            _porcupine = Porcupine.FromBuiltInKeywords(_accessKey, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            RunTestCase(
                "porcupine.wav",
                new List<int>() { 0 });
        }

        [TestMethod]
        public void TestMultipleKeywordBuiltin()
        {
            List<BuiltInKeyword> inputKeywords = new List<BuiltInKeyword>
            {
                BuiltInKeyword.ALEXA,
                BuiltInKeyword.AMERICANO,
                BuiltInKeyword.BLUEBERRY,
                BuiltInKeyword.BUMBLEBEE,
                BuiltInKeyword.GRAPEFRUIT,
                BuiltInKeyword.GRASSHOPPER,
                BuiltInKeyword.PICOVOICE,
                BuiltInKeyword.PORCUPINE,
                BuiltInKeyword.TERMINATOR
            };
            _porcupine = Porcupine.FromBuiltInKeywords(_accessKey, inputKeywords);
            RunTestCase(
                "multiple_keywords.wav",
                new List<int>() { 7, 0, 1, 2, 3, 4, 5, 6, 7, 8 });
        }

        [TestMethod]
        [DynamicData(nameof(SingleKeywordTestData))]
        public void TestSingleKeyword(string language, string keyword, string testAudio)
        {
            _porcupine = Porcupine.FromKeywordPaths(
                _accessKey,
                GetKeywordPaths(language, new List<string> { keyword }),
                GetModelPath(language)
            );

            RunTestCase(
                testAudio,
                new List<int> { 0 }
            );
        }

        [TestMethod]
        [DynamicData(nameof(MultipleKeywordTestData))]
        public void TestMultipleKeyword(string language, string[] keywords, string testAudio, int[] groundTruth)
        {
            _porcupine = Porcupine.FromKeywordPaths(
                _accessKey,
                GetKeywordPaths(language, keywords.ToList()),
                GetModelPath(language)
            );

            RunTestCase(
                testAudio,
                groundTruth.ToList()
            );
        }

        [TestMethod]
        public void TestWithNonAsciiModelName()
        {
            string language = "es";
            List<string> keywords = new List<string>() {
                "murciélago",
            };

            _porcupine = Porcupine.FromKeywordPaths(
                _accessKey,
                GetKeywordPaths(language, keywords),
                GetModelPath(language)
            );

            RunTestCase(
                "murciélago.wav",
                new List<int>() { 0, 0 });
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