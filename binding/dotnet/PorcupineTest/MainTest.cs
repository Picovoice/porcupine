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
using System.Linq;
using System.Reflection;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Pv;

namespace PorcupineTest
{
    [TestClass]
    public class MainTest
    {
        private static string ACCESS_KEY;
        private static string _relativeDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        private static Architecture _arch => RuntimeInformation.ProcessArchitecture;
        private static string _env => RuntimeInformation.IsOSPlatform(OSPlatform.OSX) ? "mac" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "windows" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Linux) && _arch == Architecture.X64 ? "linux" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Linux) &&
                                                    (_arch == Architecture.Arm || _arch == Architecture.Arm64) ? PvLinuxEnv() : "";
        private Porcupine porcupine;

        private static string PvLinuxEnv()
        {
            string cpuInfo = File.ReadAllText("/proc/cpuinfo");
            string[] cpuPartList = cpuInfo.Split('\n').Where(x => x.Contains("CPU part")).ToArray();
            if (cpuPartList.Length == 0)
                throw new PlatformNotSupportedException($"Unsupported CPU.\n{cpuInfo}");

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
        public static void ClassInitialize(TestContext testContext)
        {
            if (testContext.Properties.Contains("pvTestAccessKey"))
            {
                ACCESS_KEY = testContext.Properties["pvTestAccessKey"].ToString();
            }
        }

        private static string appendLanguage(string s, string language)
        {
            if(language == "en")
                return s;
            return $"{s}_{language}";
        }

        private static string getKeywordPath(string language, string keyword)
        {
            return Path.Combine(
                _relativeDir,
                "../../../../../../resources",
                appendLanguage("keyword_files", language),
                $"{_env}/{keyword}_{_env}.ppn"
            );
        }

        private static List<string> getKeywordPaths(string language, List<string> keywords)
        {
            List<string> keywordPaths = new List<string>();
            foreach (string keyword in keywords)
            {
                keywordPaths.Add(getKeywordPath(language, keyword));
            }
            return keywordPaths;
        }

        private static string getModelPath(string language)
        {
            string file_name = appendLanguage("porcupine_params", language);
            return Path.Combine(
                _relativeDir,
                "../../../../../../lib/common",
                $"{file_name}.pv"
            );
        }

        private void runProcess(string audioFileName, List<int> expectedResults)
        {
            int frameLen = porcupine.FrameLength;
            string testAudioPath = Path.Combine(_relativeDir, "resources/audio_samples", audioFileName);
            List<short> data = GetPcmFromFile(testAudioPath, porcupine.SampleRate);

            int framecount = (int)Math.Floor((float)(data.Count / frameLen));
            var results = new List<int>();
            for (int i = 0; i < framecount; i++)
            {
                int start = i * frameLen;
                int count = frameLen;
                List<short> frame = data.GetRange(start, count);
                int result = porcupine.Process(frame.ToArray());
                Assert.IsTrue(result >= -1, "Porcupine returned an unexpected result (<-1)");
                if (result >= 0)
                {
                    results.Add(result);
                }
            }

            Assert.AreEqual(expectedResults.Count, results.Count, $"Should have found {expectedResults.Count} keywords, but {results.Count} were found.");
            for (int i = 0; i < results.Count; i++)
            {
                Assert.AreEqual(expectedResults[i], results[i], $"Porcupine did not detect keyword #{i+1} correctly.");
            }

            porcupine.Dispose(); 
        }

        [TestMethod]
        public void TestFrameLength()
        {
            porcupine = Porcupine.FromBuiltInKeywords(ACCESS_KEY, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            Assert.IsTrue(porcupine.FrameLength > 0, "Specified frame length was not a valid number.");
            porcupine.Dispose(); 
        }

        [TestMethod]
        public void TestVersion()
        {
            porcupine = Porcupine.FromBuiltInKeywords(ACCESS_KEY, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            Assert.IsFalse(string.IsNullOrWhiteSpace(porcupine.Version), "Porcupine did not return a valid version number.");
            porcupine.Dispose(); 
        }

        [TestMethod]
        public void TestSingleKeyword()
        {
            porcupine = Porcupine.FromBuiltInKeywords(ACCESS_KEY, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            runProcess(
                "porcupine.wav",
                new List<int>() {0});
        }

        [TestMethod]
        public void TestMultipleKeyword()
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
            porcupine = Porcupine.FromBuiltInKeywords(ACCESS_KEY, inputKeywords);
            runProcess(
                "multiple_keywords.wav",
                new List<int>() {7, 0, 1, 2, 3, 4, 5, 6, 7, 8});
        }

        [TestMethod]
        public void TestSingleKeywordDe()
        {
            string language = "de";
            List<string> keywords = new List<string>() {"heuschrecke"};

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                getKeywordPaths(language, keywords),
                getModelPath(language),
                Enumerable.Repeat(0.5f, keywords.Count())
            );

            runProcess(
                "heuschrecke.wav",
                new List<int>() {0});
        }

        [TestMethod]
        public void TestMultipleKeywordDe()
        {
            string language = "de";
            List<string> keywords = new List<string>() {
                "heuschrecke",
                "ananas",
                "leguan",
                "stachelschwein"
            };

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                getKeywordPaths(language, keywords),
                getModelPath(language),
                Enumerable.Repeat(0.5f, keywords.Count())
            );

            runProcess(
                "multiple_keywords_de.wav",
                new List<int>() {1, 0, 2, 3});
        }

        [TestMethod]
        public void TestSingleKeywordEs()
        {
            string language = "es";
            List<string> keywords = new List<string>() {
                "manzana"
            };

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                getKeywordPaths(language, keywords),
                getModelPath(language),
                Enumerable.Repeat(0.5f, keywords.Count())
            );

            runProcess(
                "manzana.wav",
                new List<int>() {0});
        }

        [TestMethod]
        public void TestMultipleKeywordEs()
        {
            string language = "es";
            List<string> keywords = new List<string>() {
                "emparedado",
                "leopardo",
                "manzana"
            };

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                getKeywordPaths(language, keywords),
                getModelPath(language),
                Enumerable.Repeat(0.5f, keywords.Count())
            );

            runProcess(
                "multiple_keywords_es.wav",
                new List<int>() {0, 1, 2});
        }

        [TestMethod]
        public void TestSingleKeywordFr()
        {
            string language = "fr";
            List<string> keywords = new List<string>() {
                "mon chouchou"
            };

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                getKeywordPaths(language, keywords),
                getModelPath(language),
                Enumerable.Repeat(0.5f, keywords.Count())
            );

            runProcess(
                "mon_chouchou.wav",
                new List<int>() {0});
        }

        [TestMethod]
        public void TestMultipleKeywordFr()
        {
            string language = "fr";
            List<string> keywords = new List<string>() {
                "framboise",
                "mon chouchou",
                "parapluie"
            };

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                getKeywordPaths(language, keywords),
                getModelPath(language),
                Enumerable.Repeat(0.5f, keywords.Count())
            );

            runProcess(
                "multiple_keywords_fr.wav",
                new List<int>() {0, 1, 0, 2});
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
