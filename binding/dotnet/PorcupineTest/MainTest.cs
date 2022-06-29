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
        public static void ClassInitialize(TestContext _)
        {
            ACCESS_KEY = Environment.GetEnvironmentVariable("ACCESS_KEY");
        }

        private static string AppendLanguage(string s, string language)
        {
            if (language == "en")
                return s;
            return $"{s}_{language}";
        }

        private static string GetKeywordPath(string language, string keyword)
        {
            return Path.Combine(
                _relativeDir,
                "../../../../../../resources",
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
                _relativeDir,
                "../../../../../../lib/common",
                $"{file_name}.pv"
            );
        }

        private void RunTestCase(string audioFileName, List<int> expectedResults)
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
                Assert.AreEqual(expectedResults[i], results[i], $"Expected keyword {expectedResults[i]}, but Porcupine detected keyword {results[i]}.");
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
        public void TestSingleKeywordBuiltin()
        {
            porcupine = Porcupine.FromBuiltInKeywords(ACCESS_KEY, new List<BuiltInKeyword> { BuiltInKeyword.PORCUPINE });
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
            porcupine = Porcupine.FromBuiltInKeywords(ACCESS_KEY, inputKeywords);
            RunTestCase(
                "multiple_keywords.wav",
                new List<int>() { 7, 0, 1, 2, 3, 4, 5, 6, 7, 8 });
        }

        [TestMethod]
        [DataRow("en", "porcupine", "porcupine.wav")]
        [DataRow("de", "heuschrecke", "heuschrecke.wav")]
        [DataRow("es", "manzana", "manzana.wav")]
        [DataRow("fr", "mon chouchou", "mon_chouchou.wav")]
        public void TestSingleKeyword(string language, string keyword, string audioFileName)
        {
            List<string> keywords = new List<string>() { keyword };

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                GetKeywordPaths(language, keywords),
                GetModelPath(language)
            );

            RunTestCase(
                audioFileName,
                new List<int>() { 0 }
            );
        }

        [TestMethod]
        [DataRow("de", new string[] { "heuschrecke", "ananas", "leguan", "stachelschwein" }, new int[] { 1, 0, 2, 3 })]
        [DataRow("es", new string[] { "emparedado", "leopardo", "manzana" }, new int[] { 0, 1, 2 })]
        [DataRow("fr", new string[] { "framboise", "mon chouchou", "parapluie" }, new int[] { 0, 1, 0, 2 })]
        [DataRow("ko", new string[] { "aiseukeulim", "bigseubi", "koppulso" }, new int[] { 1, 2, 0 })]
        [DataRow("ja", new string[] { "ninja", "bushi", "ringo" }, new int[] { 2, 1, 0 })]
        [DataRow("it", new string[] { "espresso", "cameriere", "porcospino" }, new int[] { 2, 0, 1 })]
        [DataRow("pt", new string[] { "abacaxi", "fenomeno", "formiga" }, new int[] { 0, 2, 1 })]
        public void TestMultipleKeyword(string language, string[] keywords, int[] expectedResults)
        {
            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                GetKeywordPaths(language, keywords.ToList()),
                GetModelPath(language)
            );

            RunTestCase(
                String.Format("multiple_keywords_{0}.wav", language),
                expectedResults.ToList()
            );
        }

        [TestMethod]
        public void TestWithNonAsciiModelName()
        {
            string language = "es";
            List<string> keywords = new List<string>() {
                "murciélago",
            };

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
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
