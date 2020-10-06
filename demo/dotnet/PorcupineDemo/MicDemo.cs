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
    /// Microphone Demo for Porcupine wake word engine. It creates an input audio stream from a microphone, monitors it, and
    /// upon detecting the specified wake word(s) prints the detection time and wake word on console. It optionally saves
    /// the recorded audio into a file for further debugging.
    /// </summary>                
    public class MicDemo
    {
        
        /// <summary>
        ///  Creates an input audio stream, instantiates an instance of Porcupine object, and monitors the audio stream for
        ///  occurrencec of the wake word(s). It prints the time of detection for each occurrence and the wake word.
        /// </summary>
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
        /// <param name="audioDeviceIndex">Optional argument. If provided, audio is recorded from this input device. Otherwise, the default audio input device is used.</param>        
        /// <param name="outputPath">Optional argument. If provided, recorded audio will be stored in this location at the end of the run.</param>        
        /// <returns>An instance of Porcupine wake word engine.</returns>    
        public static void RunDemo(string modelPath, List<string> keywordPaths, List<string> keywords, List<float> sensitivities,
                                   int? audioDeviceIndex = null, string outputPath = null)
        {
            Porcupine porcupine = null;
            WaveInEvent audioIn = null;
            WaveFileWriter outputFileWriter = null;
            try
            {
                porcupine = Porcupine.Create(modelPath, keywordPaths, keywords, sensitivities);
                audioIn = new WaveInEvent()
                {
                    NumberOfBuffers = 3,
                    BufferMilliseconds = (int)(porcupine.FrameLength / (float)porcupine.SampleRate * 3000),
                    WaveFormat = new WaveFormat(porcupine.SampleRate, 16, 1)
                };

                if (audioDeviceIndex != null)
                    audioIn.DeviceNumber = audioDeviceIndex.Value;

                if (!string.IsNullOrWhiteSpace(outputPath))
                {
                    outputFileWriter = new WaveFileWriter(outputPath, new WaveFormat(16000, 16, 1));
                }

                audioIn.DataAvailable += (s, e) =>
                {
                    short[] pcmBuffer = new short[porcupine.FrameLength];
                    for (int idx = 0; idx < e.BytesRecorded; idx += porcupine.FrameLength * 2)
                    {
                        outputFileWriter?.Write(e.Buffer, idx, porcupine.FrameLength * 2);
                        Buffer.BlockCopy(e.Buffer, idx, pcmBuffer, 0, porcupine.FrameLength * 2);
                        int result = porcupine.Process(pcmBuffer);
                        if (result >= 0)
                        {
                            Console.WriteLine($"[{DateTime.Now.ToLongTimeString()}] Detected '{keywords[result]}'");
                        }
                    }
                };
                audioIn.RecordingStopped += (s, e) =>
                {
                    if (e.Exception != null)
                    {
                        Console.WriteLine("Recording stopped unexpectedly:\n" + e.Exception);
                    }
                };

                Console.Write("Listening for {");
                for (int i = 0; i < keywords.Count; i++)
                {
                    Console.Write($" {keywords[i]}({sensitivities[i]})");
                }
                Console.Write("  }\n");

                audioIn.StartRecording();
                Console.ReadKey();
                Console.WriteLine("Stopping...");
            }
            finally
            {
                audioIn?.StopRecording();
                outputFileWriter?.Flush();
                outputFileWriter?.Dispose();
                porcupine?.Dispose();
            }
        }

        /// <summary>
        /// Lists available audio input devices.
        /// </summary>
        public static void ShowAudioDevices()
        {
            int waveInDevices = WaveIn.DeviceCount;
            if (waveInDevices > 0)
            {
                Console.WriteLine("Available input devices: \n");
                for (int waveInDevice = 0; waveInDevice < waveInDevices; waveInDevice++)
                {
                    WaveInCapabilities deviceInfo = WaveIn.GetCapabilities(waveInDevice);
                    Console.WriteLine($"\tDevice {waveInDevice}: {deviceInfo.ProductName}, {deviceInfo.Channels} channels");
                }
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
            int? audioDeviceIndex = null;
            string outputPath = null;
            bool showAudioDevices = false;
            bool showHelp = false;

            // parse command line arguments
            int argIdx = 0;
            while (argIdx < args.Length)
            {
                if (args[argIdx] == "--keywords")
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
                    while (argIdx < args.Length && File.Exists(args[argIdx]) && args[argIdx].EndsWith(".ppn"))
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
                else if (args[argIdx] == "--show_audio_devices")
                {
                    showAudioDevices = true;
                    argIdx++;
                }
                else if (args[argIdx] == "--audio_device_index")
                {
                    int deviceIdx;
                    if (++argIdx < args.Length && int.TryParse(args[argIdx], out deviceIdx))
                    {
                        audioDeviceIndex = deviceIdx;
                        argIdx++;
                    }
                }
                else if (args[argIdx] == "--output_path")
                {
                    if (++argIdx < args.Length)
                    {
                        outputPath = args[argIdx++];
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

            // print audio device info and exit
            if (showAudioDevices)
            {
                ShowAudioDevices();                
                Console.ReadKey();
                return;
            }

            // argument validation
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
            RunDemo(modelPath, keywordPaths, keywords, sensitivities, audioDeviceIndex, outputPath);            
        }

        private static void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Console.WriteLine(e.ExceptionObject.ToString());
            Console.ReadKey();
            Environment.Exit(-1);
        }

        private static readonly string HELP_STR = "Available options: \n " +
            $"\t--keywords: List of default keywords for detection. Available keywords: {string.Join(", ", Porcupine.KEYWORDS)}\n" +
            $"\t--keyword_paths: Absolute paths to keyword model files. If not set it will be populated from `--keywords` argument\n" +
            $"\t--model_path: Absolute path to the file containing model parameters.\n" +
            $"\t--sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher \n" +
             "\t\tsensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.\n" +
            "\t--audio_device_index: Index of input audio device.\n" +
            "\t--output_path: Absolute path to recorded audio for debugging.\n" +
            "\t--show_audio_devices: Print available recording devices.\n";
    }
}
