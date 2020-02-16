using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PorcupineCS;

namespace PorcupineTest
{
    [TestClass]
    public class MainTest
    {
        private List<string> keywordPaths;
        private List<float> sensitivities;

        private string GetAbsRootPath()
        {
            return Path.GetFullPath($"{Environment.CurrentDirectory}/../../../../../../");
        }

        [TestInitialize]
        public void Init()
        {
            if (!File.Exists($"libpv_porcupine{GetExtension()}"))
                File.Copy($"{GetAbsRootPath()}lib/{GetEnvironmentName()}/{GetArchitecture()}/libpv_porcupine{GetExtension()}", $"./libpv_porcupine{GetExtension()}");

            if (!File.Exists("porcupine_params.pv"))
                File.Copy($"{GetAbsRootPath()}lib/common/porcupine_params.pv", "./porcupine_params.pv");

            if (!File.Exists("porcupine.wav"))
                File.Copy($"{GetAbsRootPath()}resources/audio_samples/porcupine.wav", "./porcupine.wav");

            if (!File.Exists("multiple_keywords.wav"))
                File.Copy($"{GetAbsRootPath()}resources/audio_samples/multiple_keywords.wav", "./multiple_keywords.wav");

            string[] keywords = {"americano", "blueberry", "bumblebee", "grapefruit", "grasshopper", "picovoice", "porcupine", "terminator"};

            keywordPaths = new List<string>();
            foreach (string keyword in keywords) {
                keywordPaths.Add($"{GetAbsRootPath()}resources/keyword_files/{GetEnvironmentName()}/{keyword}_{GetEnvironmentName()}.ppn");
                Console.WriteLine($"{GetAbsRootPath()}resources/keyword_files/{GetEnvironmentName()}/{keyword}_{GetEnvironmentName()}.ppn");
            }
            
            sensitivities = new List<float>();
            for (int i = 0; i < keywordPaths.Count; i++)
            {
                sensitivities.Add(0.5f);
            }
        }

        [TestMethod]
        public void TestProcess()
        {
            var modelFilePath = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, "porcupine_params.pv"));

            Assert.IsTrue(File.Exists(modelFilePath), $"File.Exists(modelFilePath) --> {modelFilePath}");
            keywordPaths.ForEach(keywordFilePath => Assert.IsTrue(File.Exists(keywordFilePath), $"File.Exists(keywordFilePath) --> {keywordFilePath}"));
            Porcupine p = new Porcupine(modelFilePath, keywordPaths: keywordPaths, sensitivities: sensitivities);
            
            List<short> data = new List<short>();
            using (BinaryReader reader = new BinaryReader(File.Open("multiple_keywords.wav", FileMode.Open))) {
                reader.ReadBytes(44);
                
                while (reader.BaseStream.Position != reader.BaseStream.Length) {
                    data.Add(reader.ReadInt16());
                }
            }
            
            int framecount = (int)Math.Floor((decimal)(data.Count / p.FrameLength()));
            var results = new List<int>();
            for (int i = 0; i < framecount; i++)
            {
                int start = i * p.FrameLength();
                int count = p.FrameLength();
                List<short> frame = data.GetRange(start, count);
                int result = p.Process(frame.ToArray());
                if (result >= 0)
                {
                    results.Add(result);
                }
            }

            var requiredRes = new[] { 6, 0, 1, 2, 3, 4, 5, 6, 7 };

            Assert.AreEqual(requiredRes.Length, results.Count, $"expected results length are different expected {requiredRes.Length} got {results.Count}");
            for (var i = 0; i < results.Count; i++)
            {
                Assert.AreEqual(requiredRes[i], results[i], $"The result is not as expected, expected {requiredRes[i]} got {results[i]}");
            }

            p.Dispose();
        }

        private static string GetExtension()
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                return ".dylib";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                return ".so";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return ".dll";
            }

            throw new NotImplementedException("this OS has no binding logic implemented yet.");
        }

        private static string GetEnvironmentName()
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                return "mac";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                return "linux";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return "windows";
            }

            throw new NotImplementedException("this OS has no binding logic implemented yet.");
        }


        private static string GetArchitecture()
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                return "x86_64";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                return "x86_64";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return "amd64";
            }

            throw new NotImplementedException("this OS has no binding logic implemented yet.");
        }
    }
}
