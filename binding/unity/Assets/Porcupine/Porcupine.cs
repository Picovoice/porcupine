//
// Copyright 2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Networking;

namespace Pv.Unity
{
    public class Porcupine : IDisposable
    {
        /// <summary>
        ///  Status codes returned by Porcupine library
        /// </summary>
        public enum PorcupineStatus
        {
            SUCCESS = 0,
            OUT_OF_MEMORY = 1,
            IO_ERROR = 2,
            INVALID_ARGUMENT = 3
        }

#if !UNITY_EDITOR && UNITY_IOS
        private const string LIBRARY_PATH = "__Internal";
#else
        private const string LIBRARY_PATH = "pv_porcupine";
#endif
        private IntPtr _libraryPointer = IntPtr.Zero;

        [DllImport(LIBRARY_PATH, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PorcupineStatus pv_porcupine_init(string modelPath, int numKeywords, string[] keywordPaths, float[] sensitivities, out IntPtr handle);

        [DllImport(LIBRARY_PATH, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_sample_rate();

        [DllImport(LIBRARY_PATH, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void pv_porcupine_delete(IntPtr handle);

        [DllImport(LIBRARY_PATH, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PorcupineStatus pv_porcupine_process(IntPtr handle, short[] pcm, out int keywordIndex);

        [DllImport(LIBRARY_PATH, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr pv_porcupine_version();

        [DllImport(LIBRARY_PATH, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_porcupine_frame_length();
        
        private static readonly string _platform;
        private static readonly Dictionary<string, string> _builtInKeywordPaths;
        public static readonly string DEFAULT_MODEL_PATH;
        public static readonly List<string> BUILT_IN_KEYWORDS;

        static Porcupine()
        {
            _platform = GetPlatform();            
            _builtInKeywordPaths = GetBuiltInKeywordPaths(_platform);
            BUILT_IN_KEYWORDS = _builtInKeywordPaths.Keys.ToList();
            DEFAULT_MODEL_PATH = GetDefaultModelPath();
        }

        /// <summary>
        /// Creates an instance of the Porcupine wake word engine.
        /// </summary>
        /// <param name="modelPath">Absolute path to the file containing model parameters. If not set it will be set to the default location.</param>
        /// <param name="keywordPaths"> Absolute paths to keyword model files. If not set it will be populated from `keywords` argument.</param>
        /// <param name="keywords">
        /// List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved 
        /// using `Porcupine.BUILT_IN_KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
        /// </param>
        /// <param name="sensitivities">
        /// Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer 
        /// misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.
        /// </param>
        /// <returns>An instance of Porcupine wake word engine.</returns>                             
        public static Porcupine Create(string modelPath = null, IEnumerable<string> keywordPaths = null,
                                       IEnumerable<string> keywords = null, IEnumerable<float> sensitivities = null)
        {
            modelPath = modelPath ?? DEFAULT_MODEL_PATH;
            if (!File.Exists(modelPath))
            {
                throw new IOException("Couldn't find model file at " + modelPath);
            }

            if (keywordPaths != null && keywords != null)
            {
                throw new ArgumentException("Both 'keywords' and 'keywordPaths' were set. Only one of the two arguments may be set for intializtion.");
            }

            if (keywordPaths == null)
            {
                if (keywords == null)
                {
                    throw new ArgumentNullException("keywords", "Either 'keywords' or 'keywordPaths' must be set.");
                }

                if (keywords.All(k => BUILT_IN_KEYWORDS.Contains(k)))
                {
                    keywordPaths = keywords.Select(k => _builtInKeywordPaths[k]);
                }
                else
                {
                    throw new ArgumentException("One or more keywords are not available by default. Available default keywords are:\n" +
                                                 string.Join(",", BUILT_IN_KEYWORDS.ToArray()));
                }
            }

            foreach (string path in keywordPaths)
            {
                if (!File.Exists(path))
                {
                    throw new IOException("Couldn't find keyword file at " + path);
                }
            }

            if (sensitivities == null || sensitivities.Count() == 0)
            {
                sensitivities = Enumerable.Repeat(0.5f, keywordPaths.Count());
            }
            else if (sensitivities.Any(s => s < 0 || s > 1))
            {
                throw new ArgumentException("Sensitivities should be within [0, 1].");
            }
            

            if (sensitivities.Count() != keywordPaths.Count())
            {
                throw new ArgumentException(string.Format("Number of keywords ({0}) does not match number of sensitivities ({1})", 
                    keywordPaths.Count(), sensitivities.Count()));
            }

            return new Porcupine(modelPath, keywordPaths, sensitivities);
        }

        /// <summary>
        /// Creates an instance of the Porcupine wake word engine.
        /// </summary>
        /// <param name="modelPath">Absolute path to file containing model parameters.</param>
        /// <param name="keywordPaths">A list of absolute paths to keyword model files.</param>
        /// <param name="sensitivities">
        /// A list of sensitivity values for each keyword. A higher sensitivity value lowers miss rate at the cost of increased 
        /// false alarm rate. A sensitivity value should be within [0, 1]. 
        /// </param>        
        private Porcupine(string modelPath, IEnumerable<string> keywordPaths, IEnumerable<float> sensitivities)
        {
            PorcupineStatus status = pv_porcupine_init(modelPath, keywordPaths.Count(), keywordPaths.ToArray(), sensitivities.ToArray(), out _libraryPointer);
            if (status != PorcupineStatus.SUCCESS)
            {
                throw PorcupineStatusToException(status);
            }

            Version = Marshal.PtrToStringAnsi(pv_porcupine_version());
            SampleRate = pv_sample_rate();
            FrameLength = pv_porcupine_frame_length();
        }

        /// <summary>
        /// Process a frame of audio with the wake word engine.
        /// </summary>
        /// <param name="pcm">
        /// A frame of audio samples to be assessed by Porcupine. The required audio format is found by calling `.SampleRate` to get the required 
        /// sample rate and `.FrameLength` to get the required frame size. Audio must be single-channel and 16-bit linearly-encoded.
        /// </param>
        /// <returns>
        /// Index of the detected keyword, or -1 if no detection occurred
        /// </returns>
        public int Process(short[] pcm)
        {
            if (pcm.Length != FrameLength)
            {
                throw new ArgumentException(string.Format("Input audio frame size ({0}) was not the size specified by Porcupine engine ({1}). " +
                    "Use Porcupine.FrameLength to get the correct size.", pcm.Length, FrameLength));
            }

            int keywordIndex;
            PorcupineStatus status = pv_porcupine_process(_libraryPointer, pcm, out keywordIndex);
            if (status != PorcupineStatus.SUCCESS)
            {
                throw PorcupineStatusToException(status);
            }

            return keywordIndex;
        }

        /// <summary>
        /// Get the audio sample rate required by Porcupine.
        /// </summary>
        /// <returns>Required sample rate.</returns>
        public int SampleRate { get; private set; }

        /// <summary>
        /// Gets the required number of audio samples per frame.
        /// </summary>
        /// <returns>Required frame length.</returns>
        public int FrameLength { get; private set; }

        /// <summary>
        /// Gets the version number of the Porcupine library.
        /// </summary>
        /// <returns>Version of Pocupine</returns>
        public string Version { get; private set; }


        /// <summary>
        /// Coverts status codes to relavent .NET exceptions
        /// </summary>
        /// <param name="status">Picovoice library status code.</param>
        /// <returns>.NET exception</returns>
        private static Exception PorcupineStatusToException(PorcupineStatus status)
        {
            switch (status)
            {
                case PorcupineStatus.OUT_OF_MEMORY:
                    return new OutOfMemoryException();
                case PorcupineStatus.IO_ERROR:
                    return new IOException();
                case PorcupineStatus.INVALID_ARGUMENT:
                    return new ArgumentException();
                default:
                    return new Exception(string.Format("Unmapped error code '{0}' returned from Porcupine.", status));
            }
        }

        /// <summary>
        /// Frees memory that was set aside for Porcupine
        /// </summary>
        public void Dispose()
        {
            if (_libraryPointer != IntPtr.Zero)
            {
                pv_porcupine_delete(_libraryPointer);
                _libraryPointer = IntPtr.Zero;

                // ensures finalizer doesn't trigger if already manually disposed
                GC.SuppressFinalize(this);
            }
        }

        ~Porcupine()
        {
            Dispose();
        }

        private static string GetPlatform() 
        {
            switch (Application.platform)
            {
                case RuntimePlatform.WindowsEditor:
                case RuntimePlatform.WindowsPlayer:
                    return "windows";
                case RuntimePlatform.OSXEditor:
                case RuntimePlatform.OSXPlayer:
                    return "mac";
                case RuntimePlatform.LinuxEditor:
                case RuntimePlatform.LinuxPlayer:
                    return "linux";
                case RuntimePlatform.IPhonePlayer:
                    return "ios";
                case RuntimePlatform.Android:
                    return "android";
                default:
                    throw new NotSupportedException(string.Format("Platform '{0}' not supported by Porcupine Unity binding", Application.platform));
            } 
        }

        private static string GetDefaultModelPath()
        {
#if !UNITY_EDITOR && UNITY_ANDROID
            return ExtractResource("porcupine_params.pv");
#else
            return Path.Combine(Application.streamingAssetsPath, "porcupine_params.pv");
#endif
        }

        private static Dictionary<string, string> GetBuiltInKeywordPaths(string platform)
        {
#if !UNITY_EDITOR && UNITY_ANDROID
            string[] builtInKeywords = new string[] { "alexa", "americano", "blueberry", "bumblebee", "computer", "grapefruit", "grasshopper", 
                                                      "hey google", "hey siri", "jarvis", "ok google", "picovoice", "porcupine", "terminator" };
            string keywordFilesDir = Path.Combine(Path.Combine(Application.persistentDataPath, "keyword_files"), platform);
            if (!Directory.Exists(keywordFilesDir))
            {
                Directory.CreateDirectory(keywordFilesDir);
            }

            foreach (string keyword in builtInKeywords) 
            {
                ExtractResource(Path.Combine(Path.Combine("keyword_files", platform), string.Format("{0}_{1}.ppn", keyword, platform)));
            }            
#else
            string keywordFilesDir = Path.Combine(Path.Combine(Application.streamingAssetsPath, "keyword_files"), platform); 
#endif

            Dictionary<string, string> keywordPaths = new Dictionary<string, string>();
            foreach (string keywordFile in Directory.GetFiles(keywordFilesDir, "*.ppn"))
            {
                keywordPaths.Add(Path.GetFileName(keywordFile).Split('_')[0], Path.Combine(keywordFilesDir, keywordFile));
            }

            return keywordPaths;
        }

#if !UNITY_EDITOR && UNITY_ANDROID
        public static string ExtractResource(string filePath)
        {
            string srcPath = Path.Combine(Application.streamingAssetsPath, filePath);
            string dstPath = Path.Combine(Application.persistentDataPath, filePath);
            var loadingRequest = UnityWebRequest.Get(srcPath);
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
        }
#endif

    }
}