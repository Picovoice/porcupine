/*
    Copyright 2020 Picovoice Inc.

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

using NAudio.Wave;
using Picovoice;

namespace PorcupineDemo
{
    /// <summary>
    /// File Demo for Porcupine wake word engine. It takes an input audio file and a list of keywords to search for
    /// and prints the where in the file it detects instances of the keywords.
    /// </summary>                
    public class FileDemo
    {

        /// <summary>
        /// Reads through input file and, upon detecting the specified wake word(s), prints the detection timecode and the wake word.
        /// </summary>
        /// <param name="inputAudioPath">Required arguement. Absolute path to input audio file.</param>                
        /// <param name="modelPath">Absolute path to the file containing model parameters. If not set it will be set to the default location.</param>
        /// <param name="keywordPaths">Absolute paths to keyword model files. If not set it will be populated from `keywords` argument.</param>     
        /// <param name="keywordPaths">Absolute paths to keyword model files. If not set it will be populated from `keywords` argument.</param>     
        /// <param name="sensitivities">
        /// Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer 
        /// misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.
        /// </param>
        /// <param name="keywords">
        /// List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved 
        /// using `Porcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
        /// </param>        
        public static void RunDemo(string inputAudioPath, string modelPath, List<string> keywordPaths, List<string> keywords, List<float> sensitivities)
        {
            Porcupine porcupine = null;
            WaveFileReader fileReader = null;
            try
            {
                porcupine = Porcupine.Create(modelPath, keywordPaths, keywords, sensitivities);
                fileReader = new WaveFileReader(inputAudioPath);

                if (fileReader.WaveFormat.SampleRate != porcupine.SampleRate)
                {
                    throw new ArgumentException("SampleRate",
                        $"Audio file should have a sample rate of {porcupine.SampleRate} got {fileReader.WaveFormat.SampleRate}");
                }

                bool readLeftChannelOnly = false;                
                if (fileReader.WaveFormat.Channels == 2)
                {
                    Console.WriteLine("Picovoice processes single-channel audio but stereo file is provided. Processing left channel only.");
                    readLeftChannelOnly = true;
                }
                
                byte[] rawBuffer = new byte[porcupine.FrameLength * 2 * fileReader.WaveFormat.Channels];
                short[] sampleBuffer = new short[porcupine.FrameLength];

                int nread;
                long totalSamplesRead = 0;
                int step = readLeftChannelOnly ? 4 : 2;
                while ((nread = fileReader.Read(rawBuffer, 0, rawBuffer.Length)) == rawBuffer.Length)
                {
                    for (int i = 0; i < nread; i += step) 
                    {
                        sampleBuffer[i / step] = BitConverter.ToInt16(rawBuffer, i);                         
                    }
                    totalSamplesRead += porcupine.FrameLength;

                    int result = porcupine.Process(sampleBuffer);
                    if (result >= 0)
                    {
                        Console.WriteLine($"Detected {keywords[result]} at " +
                            $"{Math.Round(totalSamplesRead / (double)fileReader.WaveFormat.SampleRate, 2)} sec");
                    }
                }
            }
            finally
            {
                porcupine?.Dispose();
                fileReader?.Dispose();
            }
        }    

        public static void Main(string[] args)
        {
            AppDomain.CurrentDomain.UnhandledException += OnUnhandledException;
            if (args.Length == 0) 
            {
                Console.WriteLine(HELP_STR);
                Console.ReadKey();
                return;
            }

            List<string> keywords = null;
            List<string> keywordPaths = null;
            List<float> sensitivities = null;
            string modelPath = null;            
            string inputAudioPath = null;            
            bool showHelp = false;

            // parse command line arguments
            int argIdx = 0;
            while (argIdx < args.Length)
            {
                if (args[argIdx] == "--input_audio_path")
                {
                    if (++argIdx < args.Length)
                    {
                        inputAudioPath = args[argIdx++];
                    }
                }
                else if (args[argIdx] == "--keywords")
                {
                    argIdx++;
                    keywords = new List<string>();
                    while (argIdx < args.Length && Porcupine.KEYWORDS.Contains(args[argIdx]))
                    {
                        keywords.Add(args[argIdx++]);
                    }                    
                }
                else if (args[argIdx] == "--keyword_paths")
                {
                    argIdx++;
                    keywordPaths = new List<string>();
                    while (argIdx < args.Length && File.Exists(args[argIdx]))
                    {
                        keywordPaths.Add(args[argIdx++]);
                    }                    
                }
                else if (args[argIdx] == "--model_path")
                {
                    if (++argIdx < args.Length && File.Exists(args[argIdx]))
                    {
                        modelPath = args[argIdx++];
                    }
                }
                else if (args[argIdx] == "--sensitivities")
                {
                    argIdx++;
                    sensitivities = new List<float>();
                    float f;
                    while (argIdx < args.Length && float.TryParse(args[argIdx], out f))
                    {
                        sensitivities.Add(f);
                        argIdx++;
                    }                    
                }                
                else if (args[argIdx] == "-h" || args[argIdx] == "--help")
                {
                    showHelp = true;
                    argIdx++;
                }
                else
                {
                    argIdx++;
                }
            }

            // print help text and exit
            if (showHelp)
            {
                Console.WriteLine(HELP_STR);
                Console.ReadKey();
                return;
            }

            // argument validation
            if (string.IsNullOrEmpty(inputAudioPath))
            {
                throw new ArgumentNullException("input_audio_path");
            }
            if (!File.Exists(inputAudioPath)) 
            {
                throw new ArgumentException("Provided input audio file does not exist.");
            }

            modelPath = modelPath ?? Porcupine.MODEL_PATH;
            
            if (keywordPaths == null || keywordPaths.Count == 0)
            {
                if (keywords == null || keywords.Count == 0)
                {
                    throw new ArgumentNullException("keywords", "Either '--keywords' or '--keyword_paths' must be set.");
                }

                if (keywords.All(k => Porcupine.KEYWORDS.Contains(k)))
                {
                    keywordPaths = keywords.Select(k => Porcupine.KEYWORD_PATHS[k]).ToList();
                }
                else
                {
                    throw new ArgumentException("One or more keywords are not available by default. Available default keywords are:\n" +
                                                 string.Join(",", Porcupine.KEYWORDS));
                }
            }

            if (sensitivities == null)
            {
                sensitivities = Enumerable.Repeat(0.5f, keywords.Count()).ToList();
            }

            if (sensitivities.Count() != keywords.Count())
            {
                throw new ArgumentException($"Number of keywords ({keywords.Count()}) does not match number of sensitivities ({sensitivities.Count()})");
            }

            // run demo with validated arguments
            RunDemo(inputAudioPath, modelPath, keywordPaths, keywords, sensitivities);
        }

        private static void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Console.WriteLine(e.ExceptionObject.ToString());
            Console.ReadKey();
            Environment.Exit(-1);
        }

        private static readonly string HELP_STR = "Available options: \n " +
            $"\t--input_audio_path (required): Absolute path to input audio file.\n" +
            $"\t--keywords: List of default keywords for detection. Available keywords: {string.Join(", ", Porcupine.KEYWORDS)}\n" +
            $"\t--keyword_paths: Absolute paths to keyword model files. If not set it will be populated from `--keywords` argument\n" +
            $"\t--model_path: Absolute path to the file containing model parameters.\n" +
            $"\t--sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher \n" +
             "\t\tsensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.\n";
    }
}
