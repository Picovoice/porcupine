/*
    Copyright 2020-2022 Picovoice Inc.

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
using System.Text;
using System.Threading;

using Pv;

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
        /// Creates an input audio stream, instantiates an instance of Porcupine object, and monitors the audio stream for
        /// occurrences of the wake word(s). It prints the time of detection for each occurrence and the wake word.
        /// </summary>
        /// <param name="accessKey">AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).</param>
        /// <param name="modelPath">Absolute path to the file containing model parameters. If not set it will be set to the default location.</param>
        /// <param name="keywordPaths">Absolute paths to keyword model files. If not set it will be populated from `keywords` argument.</param>     
        /// <param name="sensitivities">
        /// Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer 
        /// misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.
        /// </param>        
        /// <param name="audioDeviceIndex">Optional argument. If provided, audio is recorded from this input device. Otherwise, the default audio input device is used.</param>        
        /// <param name="outputPath">Optional argument. If provided, recorded audio will be stored in this location at the end of the run.</param>        
        public static void RunDemo(
            string accessKey,
            string modelPath,
            List<string> keywordPaths,
            List<float> sensitivities,
            int audioDeviceIndex,
            string outputPath = null)
        {
            Porcupine porcupine = null;
            BinaryWriter outputFileWriter = null;
            int totalSamplesWritten = 0;

            // init porcupine wake word engine
            porcupine = Porcupine.FromKeywordPaths(accessKey, keywordPaths, modelPath, sensitivities);

            // get keyword names for labeling detection results                
            List<string> keywordNames = keywordPaths.Select(k => Path.GetFileNameWithoutExtension(k).Split("_")[0]).ToList();

            // open stream to output file
            if (!string.IsNullOrWhiteSpace(outputPath))
            {
                outputFileWriter = new BinaryWriter(new FileStream(outputPath, FileMode.OpenOrCreate, FileAccess.Write));
                WriteWavHeader(outputFileWriter, 1, 16, 16000, 0);
            }

            Console.CancelKeyPress += (s, o) =>
            {
                Console.WriteLine("Stopping...");

                if (outputFileWriter != null)
                {
                    // write size to header and clean up
                    WriteWavHeader(outputFileWriter, 1, 16, 16000, totalSamplesWritten);
                    outputFileWriter.Flush();
                    outputFileWriter.Dispose();
                }
                porcupine?.Dispose();
            };

            // create and start recording
            using PvRecorder recorder = PvRecorder.Create(deviceIndex: audioDeviceIndex, frameLength: porcupine.FrameLength);
            Console.WriteLine($"Using device: {recorder.SelectedDevice}");
            Console.Write($"Listening for [{string.Join(' ', keywordNames.Select(k => $"'{k}'"))}]...\n");
            recorder.Start();

            while (true)
            {
                short[] pcm = recorder.Read();

                int result = porcupine.Process(pcm);
                if (result >= 0)
                {
                    Console.WriteLine($"[{DateTime.Now.ToLongTimeString()}] Detected '{keywordNames[result]}'");
                }

                if (outputFileWriter != null)
                {
                    foreach (short sample in pcm)
                    {
                        outputFileWriter.Write(sample);
                    }
                    totalSamplesWritten += pcm.Length;
                }
                _ = Thread.Yield();
            }
        }

        /// <summary>
        /// Writes the RIFF header for a file in WAV format
        /// </summary>
        /// <param name="writer">Output stream to WAV file</param>
        /// <param name="channelCount">Number of channels</param>     
        /// <param name="bitDepth">Number of bits per sample</param>     
        /// <param name="sampleRate">Sampling rate in Hz</param>
        /// <param name="totalSampleCount">Total number of samples written to the file</param>
        private static void WriteWavHeader(BinaryWriter writer, ushort channelCount, ushort bitDepth, int sampleRate, int totalSampleCount)
        {
            if (writer == null)
            {
                return;
            }

            _ = writer.Seek(0, SeekOrigin.Begin);
            writer.Write(Encoding.ASCII.GetBytes("RIFF"));
            writer.Write((bitDepth / 8 * totalSampleCount) + 36);
            writer.Write(Encoding.ASCII.GetBytes("WAVE"));
            writer.Write(Encoding.ASCII.GetBytes("fmt "));
            writer.Write(16);
            writer.Write((ushort)1);
            writer.Write(channelCount);
            writer.Write(sampleRate);
            writer.Write(sampleRate * channelCount * bitDepth / 8);
            writer.Write((ushort)(channelCount * bitDepth / 8));
            writer.Write(bitDepth);
            writer.Write(Encoding.ASCII.GetBytes("data"));
            writer.Write(bitDepth / 8 * totalSampleCount);
        }

        /// <summary>
        /// Lists available audio input devices.
        /// </summary>
        public static void ShowAudioDevices()
        {
            string[] devices = PvRecorder.GetAudioDevices();
            for (int i = 0; i < devices.Length; i++)
            {
                Console.WriteLine($"index: {i}, device name: {devices[i]}");
            }
        }

        public static void Main(string[] args)
        {
            AppDomain.CurrentDomain.UnhandledException += OnUnhandledException;
            if (args.Length == 0)
            {
                Console.WriteLine(HELP_STR);
                _ = Console.Read();
                return;
            }

            List<string> keywords = null;
            List<string> keywordPaths = null;
            List<float> sensitivities = null;
            string accessKey = null;
            string modelPath = null;
            int audioDeviceIndex = -1;
            string outputPath = null;
            bool showAudioDevices = false;
            bool showHelp = false;

            // parse command line arguments
            int argIndex = 0;
            while (argIndex < args.Length)
            {
                if (args[argIndex] == "--access_key")
                {
                    if (++argIndex < args.Length)
                    {
                        accessKey = args[argIndex++];
                    }
                }
                else if (args[argIndex] == "--keywords")
                {
                    argIndex++;
                    keywords = new List<string>();
                    while (argIndex < args.Length && !args[argIndex].StartsWith("--"))
                    {
                        keywords.Add(args[argIndex++]);
                    }
                }
                else if (args[argIndex] == "--keyword_paths")
                {
                    argIndex++;
                    keywordPaths = new List<string>();
                    while (argIndex < args.Length && !args[argIndex].StartsWith("--"))
                    {
                        keywordPaths.Add(args[argIndex++]);
                    }
                }
                else if (args[argIndex] == "--model_path")
                {
                    if (++argIndex < args.Length)
                    {
                        modelPath = args[argIndex++];
                    }
                }
                else if (args[argIndex] == "--sensitivities")
                {
                    argIndex++;
                    sensitivities = new List<float>();
                    while (argIndex < args.Length && !args[argIndex].StartsWith("--") &&
                           float.TryParse(args[argIndex], out float sensitivity))
                    {
                        sensitivities.Add(sensitivity);
                        argIndex++;
                    }
                }
                else if (args[argIndex] == "--show_audio_devices")
                {
                    showAudioDevices = true;
                    argIndex++;
                }
                else if (args[argIndex] == "--audio_device_index")
                {
                    if (++argIndex < args.Length && int.TryParse(args[argIndex], out int deviceIdx))
                    {
                        audioDeviceIndex = deviceIdx;
                        argIndex++;
                    }
                }
                else if (args[argIndex] == "--output_path")
                {
                    if (++argIndex < args.Length)
                    {
                        outputPath = args[argIndex++];
                    }
                }
                else if (args[argIndex] == "-h" || args[argIndex] == "--help")
                {
                    showHelp = true;
                    argIndex++;
                }
                else
                {
                    argIndex++;
                }
            }

            // print help text and exit
            if (showHelp)
            {
                Console.WriteLine(HELP_STR);
                _ = Console.Read();
                return;
            }

            // print audio device info and exit
            if (showAudioDevices)
            {
                ShowAudioDevices();
                _ = Console.Read();
                return;
            }


            if ((keywordPaths == null || keywordPaths.Count == 0) && (keywords == null || keywords.Count == 0))
            {
                throw new ArgumentException("Either '--keywords' or '--keyword_paths' must be set.");
            }

            if (keywords != null)
            {
                keywordPaths = new List<string>();
                foreach (string k in keywords)
                {
                    if (!Enum.TryParse(typeof(BuiltInKeyword), k.ToUpper().Replace(" ", "_"), out object builtin))
                    {
                        throw new ArgumentException($"Keyword '{k}' is not a valid built-in keyword. Available built-ins are: " +
                            $"{string.Join(",", Enum.GetNames(typeof(BuiltInKeyword)).Select(k => k.ToLower().Replace("_", " ")))}");
                    }
                    else
                    {
                        keywordPaths.Add(Porcupine.BUILT_IN_KEYWORD_PATHS[(BuiltInKeyword)builtin]);
                    }
                }
            }

            // run demo with validated arguments
            RunDemo(accessKey, modelPath, keywordPaths, sensitivities, audioDeviceIndex, outputPath);
        }

        private static void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Console.WriteLine(e.ExceptionObject.ToString());
            Environment.Exit(1);
        }

        private static readonly string HELP_STR = "Available options: \n " +
            $"\t--access_key (required): AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)\n" +
            $"\t--keywords: List of built-in keywords for detection." +
            $"\t\tAvailable keywords: {string.Join(",", Enum.GetNames(typeof(BuiltInKeyword)).Select(k => k.ToLower().Replace("_", " ")))}\n" +
            $"\t--keyword_paths: Absolute paths to keyword model files. If not set it will be populated from `--keywords` argument\n" +
            $"\t--model_path: Absolute path to the file containing model parameters.\n" +
            $"\t--sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher \n" +
             "\t\tsensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.\n" +
            "\t--audio_device_index: Index of input audio device.\n" +
            "\t--output_path: Absolute path to recorded audio for debugging.\n" +
            "\t--show_audio_devices: Print available recording devices.\n";
    }
}