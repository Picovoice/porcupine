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
            return Path.Combine(new string[] {
                _relativeDir,
                "../../../../../../resources",
                appendLanguage("keyword_files", language),
                _env,
                $"{keyword}_{_env}.ppn"
            });
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
            return Path.Combine(new string[] {
                _relativeDir,
                "../../../../../../lib/common",
                $"{file_name}.pv"
            });
        }

        private Porcupine createPorcupineWrapper(string language, List<string> keywords)
        {
            List<string> keywordPaths = getKeywordPaths(language, keywords);
            IEnumerable<float> sensitivities = Enumerable.Repeat(0.5f, keywords.Count());
            return Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                getKeywordPaths(language, keywords),
                getModelPath(language),
                Enumerable.Repeat(0.5f, keywords.Count())
            );
        }

        private void runProcess(Porcupine p, string audioFileName, List<int> expectedResults)
        {
            int frameLen = p.FrameLength;
            string testAudioPath = Path.Combine(_relativeDir, "resources/audio_samples", audioFileName);
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
                Assert.AreEqual(expectedResults[i], results[i], $"Porcupine did not detect keyword #{i+1} correctly.");
            }

            p.Dispose(); 
        }

        [TestMethod]
        public void TestFrameLength()
        {
            using Porcupine p = Porcupine.FromBuiltInKeywords(ACCESS_KEY, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            Assert.IsTrue(p.FrameLength > 0, "Specified frame length was not a valid number.");
        }

        [TestMethod]
        public void TestVersion()
        {
            using Porcupine p = Porcupine.FromBuiltInKeywords(ACCESS_KEY, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            Assert.IsFalse(string.IsNullOrWhiteSpace(p.Version), "Porcupine did not return a valid version number.");
        }

        [TestMethod]
        public void TestSingleKeyword()
        {
            using Porcupine p = Porcupine.FromBuiltInKeywords(ACCESS_KEY, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
            runProcess(
                p,
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
            using Porcupine p = Porcupine.FromBuiltInKeywords(ACCESS_KEY, inputKeywords);
            runProcess(
                p,
                "multiple_keywords.wav",
                new List<int>() {7, 0, 1, 2, 3, 4, 5, 6, 7, 8});
        }

        [TestMethod]
        public void TestSingleKeywordDe()
        {
            List<string> keywords = new List<string>() {
                "heuschrecke"
            };

            using Porcupine p = createPorcupineWrapper(
                "de",
                keywords
            );

            runProcess(
                p,
                "heuschrecke.wav",
                new List<int>() {0});
        }

        [TestMethod]
        public void TestMultipleKeywordDe()
        {
            List<string> keywords = new List<string>() {
                "heuschrecke",
                "ananas",
                "leguan",
                "stachelschwein"
            };

            using Porcupine p = createPorcupineWrapper(
                "de",
                keywords
            );

            runProcess(
                p,
                "multiple_keywords_de.wav",
                new List<int>() {1, 0, 2, 3});
        }

        [TestMethod]
        public void TestSingleKeywordEs()
        {
            List<string> keywords = new List<string>() {
                "manzana"
            };

            using Porcupine p = createPorcupineWrapper(
                "es",
                keywords
            );

            runProcess(
                p,
                "manzana.wav",
                new List<int>() {0});
        }

        [TestMethod]
        public void TestMultipleKeywordEs()
        {
            List<string> keywords = new List<string>() {
                "emparedado",
                "leopardo",
                "manzana"
            };

            using Porcupine p = createPorcupineWrapper(
                "es",
                keywords
            );

            runProcess(
                p,
                "multiple_keywords_es.wav",
                new List<int>() {0, 1, 2});
        }

        [TestMethod]
        public void TestSingleKeywordFr()
        {
            List<string> keywords = new List<string>() {
                "mon chouchou"
            };

            using Porcupine p = createPorcupineWrapper(
                "fr",
                keywords
            );

            runProcess(
                p,
                "mon_chouchou.wav",
                new List<int>() {0});
        }

        [TestMethod]
        public void TestMultipleKeywordFr()
        {
            List<string> keywords = new List<string>() {
                "framboise",
                "mon chouchou",
                "parapluie"
            };

            using Porcupine p = createPorcupineWrapper(
                "fr",
                keywords
            );

            runProcess(
                p,
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
