using System;
using System.Collections.Generic;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PorcupineCS;

namespace PorcupineTest
{
    [TestClass]
    public class MainTest
    {
        private List<string> paths;
        private List<float> senses;

        private string GetAbsRootPath()
        {
            return $"{Environment.CurrentDirectory}/../../../../../../";
        }

        [TestInitialize]
        public void Init()
        {
            Console.WriteLine($"{Environment.CurrentDirectory}");
            if (!File.Exists("libpv_porcupine.dll"))
                File.Copy($"/../../../../../../lib/windows/amd64/libpv_porcupine.dll", "./libpv_porcupine.dll");

            if(!File.Exists("porcupine_params.pv"))
                File.Copy("../../../../../../lib/common/porcupine_params.pv", "./porcupine_params.pv");

            if (!File.Exists("porcupine.wav"))
                File.Copy("../../../../../../resources/audio_samples/porcupine.wav", "./porcupine.wav");

            paths = new List<string>();
            List<string> temp = new List<string>()
            {
                "alexa", "americano", "avocado", "blueberry", "bumblebee", "caterpillar", "christina",
                "dragonfly", "flamingo", "francesca", "grapefruit", "grasshopper", "iguana", "picovoice",
                "pineapple", "porcupine", "raspberry", "terminator", "vancouver"
            };
            foreach (string name in temp)
            {
                paths.Add($"{GetAbsRootPath()}resources/keyword_files/{name}_windows.ppn");
            }
            senses = new List<float>();
            for (int i= 0; i < paths.Count; i++)
            {
                senses.Add(0.5f);
            }

            //Console.WriteLine();
        }

        [TestMethod]
        public void TestMethod1()
        {
            Porcupine p = new Porcupine($"{Environment.CurrentDirectory}/porcupine_params.pv", keywordFilePaths:paths, sensitivities: senses);
            short[] data = OpenWav("porcupine.wav", out short[] _); // the underscore indicates unnecessary data
            PicoVoiceStatus status = p.ProcessMultipleKeywords(data, out int result);
            Assert.AreEqual(PicoVoiceStatus.SUCCESS, status, "The status is not as expected");
            Assert.AreEqual(0, result, "The result is not as expected");
        }

        #region utility

        // convert two bytes to one double in the range -1 to 1
        private static short BytesToShort(byte firstByte, byte secondByte)
        {
            // convert two bytes to one short (little endian)
            short s = (short)((secondByte << 8) | firstByte);
            // convert to range from -1 to (just below) 1
            return (short)(s + 32768); /// 32768.0;
        }

        // Returns left and right double arrays. 'right' will be null if sound is mono.
        private static short[] OpenWav(string filename, out short[] right)
        {
            byte[] wav = File.ReadAllBytes(filename);


            // Determine if mono or stereo
            int channels = wav[22];     // Forget byte 23 as 99.999% of WAVs are 1 or 2 channels

            // Get past all the other sub chunks to get to the data subchunk:
            int pos = 12;   // First Subchunk ID from 12 to 16

            // Keep iterating until we find the data chunk (i.e. 64 61 74 61 ...... (i.e. 100 97 116 97 in decimal))
            while (!(wav[pos] == 100 && wav[pos + 1] == 97 && wav[pos + 2] == 116 && wav[pos + 3] == 97))
            {
                pos += 4;
                int chunkSize = wav[pos] + wav[pos + 1] * 256 + wav[pos + 2] * 65536 + wav[pos + 3] * 16777216;
                pos += 4 + chunkSize;
            }
            pos += 8;

            // Pos is now positioned to start of actual sound data.
            int samples = (wav.Length - pos) / 2;     // 2 bytes per sample (16 bit sound mono)
            if (channels == 2) samples /= 2;        // 4 bytes per sample (16 bit stereo)

            short[] left = new short[samples];
            // Allocate memory (right will be null if only mono sound)
            left = new short[samples];
            if (channels == 2) right = new short[samples];
            else right = null;

            // Write to double array/s:
            int i = 0;
            while (pos < samples)
            {
                left[i] = BytesToShort(wav[pos], wav[pos + 1]);
                pos += 2;
                if (channels == 2)
                {
                    right[i] = BytesToShort(wav[pos], wav[pos + 1]);
                    pos += 2;
                }
                i++;
            }

            return left;
        } 
        #endregion
    }
}
