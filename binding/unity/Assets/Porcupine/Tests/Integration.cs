using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;

using NUnit.Framework;

using UnityEngine;
using UnityEngine.TestTools;

#if !UNITY_EDITOR && UNITY_ANDROID

using UnityEngine.Networking;

#endif

using Pv.Unity;

namespace Tests
{
    [Serializable]
    public class TestData
    {
        public Tests tests;
    }

    [Serializable]
    public class Tests
    {
        public SingleKeywordTest[] singleKeyword;
        public MultipleKeywordTest[] multipleKeyword;
    }

    [Serializable]
    public class SingleKeywordTest
    {
        public string language;
        public string wakeword;
    }

    [Serializable]
    public class MultipleKeywordTest
    {
        public string language;
        public string[] wakewords;
        public int[] groundTruth;
    }

    public class Integration
    {
        private static string ACCESS_KEY = "{TESTING_ACCESS_KEY_HERE}";
        private Porcupine porcupine;

#if !UNITY_EDITOR && UNITY_ANDROID

        private static string _env = "android";

#elif !UNITY_EDITOR && UNITY_IOS

        private static string _env = "ios";

#elif UNITY_STANDALONE_LINUX || UNITY_EDITOR_LINUX

        private static string _env = "linux";

#elif UNITY_STANDALONE_OSX || UNITY_EDITOR_OSX

        private static string _env = "mac";

#elif UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN

        private static string _env = "windows";

#else

#error

#endif

        public static string ExtractResource(string filePath)
        {

#if !UNITY_EDITOR && UNITY_ANDROID

            string dstPath = filePath.Replace(Application.streamingAssetsPath, Application.persistentDataPath);
            string dstDir = Path.GetDirectoryName(dstPath);
            if (!Directory.Exists(dstDir))
            {
                Directory.CreateDirectory(dstDir);
            }

            var loadingRequest = UnityWebRequest.Get(filePath);
            loadingRequest.SendWebRequest();

            while (!loadingRequest.isDone)
            {
                if (loadingRequest.isNetworkError || loadingRequest.isHttpError)
                {
                    break;
                }
            }
            if (!(loadingRequest.isNetworkError || loadingRequest.isHttpError))
            {
                File.WriteAllBytes(dstPath, loadingRequest.downloadHandler.data);
            }

            return dstPath;

#else

            return filePath;

#endif
        }

        private static TestData LoadJsonTestData()
        {
            string dataAsJson = File.ReadAllText(ExtractResource(Path.Combine(Application.streamingAssetsPath, "test/test_data.json")));
            return JsonUtility.FromJson<TestData>(dataAsJson);
        }

        static SingleKeywordTest[] SingleKeywordTestData()
        {
            TestData testData = LoadJsonTestData();
            return testData.tests.singleKeyword;
        }

        static MultipleKeywordTest[] MultipleKeywordTestData()
        {
            TestData testData = LoadJsonTestData();
            return testData.tests.multipleKeyword;
        }

        private List<short> GetPcmFromFile(string audioFilePath, int expectedSampleRate)
        {
            List<short> data = new List<short>();
            using (BinaryReader reader = new BinaryReader(File.OpenRead(audioFilePath)))
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
            string filepath = Path.Combine(
                Application.streamingAssetsPath,
                "test",
                AppendLanguage("keyword_files", language),
                $"{_env}/{keyword}_{_env}.ppn"
            );
            return ExtractResource(filepath);
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
            string filename = AppendLanguage("porcupine_params", language);
            string filepath = Path.Combine(
                Application.streamingAssetsPath,
                "test",
                "model_files",
                $"{filename}.pv"
            );
            return ExtractResource(filepath);
        }

        private void RunTestCase(string audioFileName, List<int> expectedResults)
        {
            int frameLen = porcupine.FrameLength;
            string testAudioPath = ExtractResource(Path.Combine(Application.streamingAssetsPath, "test/audio_samples", audioFileName));
            List<short> data = GetPcmFromFile(testAudioPath, porcupine.SampleRate);

            int framecount = (int)Math.Floor((float)(data.Count / frameLen));
            List<int> results = new List<int>();
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

        [Test]
        public void SingleKeyword([ValueSource("SingleKeywordTestData")] SingleKeywordTest testCase)
        {
            List<string> keywords = new List<string>() { testCase.wakeword };

            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                GetKeywordPaths(testCase.language, keywords),
                GetModelPath(testCase.language)
            );

            RunTestCase(
                $"{testCase.wakeword.Replace(' ', '_')}.wav",
                new List<int>() { 0 }
            );
        }

        [Test]
        public void MultipleKeyword([ValueSource("MultipleKeywordTestData")] MultipleKeywordTest testCase)
        {
            porcupine = Porcupine.FromKeywordPaths(
                ACCESS_KEY,
                GetKeywordPaths(testCase.language, testCase.wakewords.ToList()),
                GetModelPath(testCase.language)
            );

            RunTestCase(
                String.Format("{0}.wav", AppendLanguage("multiple_keywords", testCase.language)),
                testCase.groundTruth.ToList()
            );
        }
    }
}
