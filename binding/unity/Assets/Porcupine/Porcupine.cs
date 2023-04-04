//
// Copyright 2021-2023 Picovoice Inc.
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
using System.Runtime.InteropServices;
using UnityEngine;

#if !UNITY_EDITOR && UNITY_ANDROID

using UnityEngine.Networking;

#endif

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
            INVALID_ARGUMENT = 3,
            STOP_ITERATION = 4,
            KEY_ERROR = 5,
            INVALID_STATE = 6,
            RUNTIME_ERROR = 7,
            ACTIVATION_ERROR = 8,
            ACTIVATION_LIMIT_REACHED = 9,
            ACTIVATION_THROTTLED = 10,
            ACTIVATION_REFUSED = 11
        }

        /// <summary>
        ///  Built-in keywords
        /// </summary>
        public enum BuiltInKeyword
        {
            ALEXA,
            AMERICANO,
            BLUEBERRY,
            BUMBLEBEE,
            COMPUTER,
            GRAPEFRUIT,
            GRASSHOPPER,
            HEY_GOOGLE,
            HEY_SIRI,
            JARVIS,
            OK_GOOGLE,
            PICOVOICE,
            PORCUPINE,
            TERMINATOR
        }

#if !UNITY_EDITOR && UNITY_IOS

        private const string LIBRARY_PATH = "__Internal";

#else

        private const string LIBRARY_PATH = "pv_porcupine";

#endif

        private IntPtr _libraryPointer = IntPtr.Zero;

        [DllImport(LIBRARY_PATH, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PorcupineStatus pv_porcupine_init(string accessKey, string modelPath, int numKeywords, string[] keywordPaths, float[] sensitivities, out IntPtr handle);

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
        private static readonly Dictionary<BuiltInKeyword, string> _builtInKeywordPaths;
        public static readonly string DEFAULT_MODEL_PATH;

        static Porcupine()
        {
            _platform = GetPlatform();
            _builtInKeywordPaths = GetBuiltInKeywordPaths(_platform);
            DEFAULT_MODEL_PATH = GetDefaultModelPath();
        }

        /// <summary>
        /// Creates an instance of the Porcupine wake word engine from built-in keywords.
        /// </summary>
        /// <param name="accessKey">AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).</param>
        /// <param name="modelPath">Absolute path to the file containing model parameters. If not set it will be set to the default location.</param>
        /// <param name="keywords">List of built-in keywords for detection.</param>
        /// <param name="sensitivities">
        /// Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer 
        /// misses at the cost of increasing the false alarm rate. If not set, 0.5 will be used.
        /// </param>
        /// <returns>An instance of Porcupine wake word engine.</returns> 
        public static Porcupine FromBuiltInKeywords(
            string accessKey,
            IEnumerable<BuiltInKeyword> keywords,
            string modelPath = null,
            IEnumerable<float> sensitivities = null)
        {
            if (keywords == null || keywords.Count() == 0)
            {
                throw new PorcupineInvalidArgumentException("No built-in keywords were specified.");
            }

            IEnumerable<string> keywordPaths = keywords
                .Where(k => _builtInKeywordPaths.ContainsKey(k))
                .Select(k => _builtInKeywordPaths[k]);

            return new Porcupine(accessKey, modelPath, keywordPaths, sensitivities);
        }

        /// <summary>
        /// Creates an instance of the Porcupine wake word engine.
        /// </summary>
        /// <param name="accessKey">AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).</param>
        /// <param name="modelPath">Absolute path to file containing model parameters.</param>
        /// <param name="keywordPaths">A list of absolute paths to keyword model files.</param>
        /// <param name="sensitivities">
        /// A list of sensitivity values for each keyword. A higher sensitivity value lowers miss rate at the cost of increased 
        /// false alarm rate. A sensitivity value should be within [0, 1]. 
        /// </param>  
        public static Porcupine FromKeywordPaths(
            string accessKey,
            IEnumerable<string> keywordPaths,
            string modelPath = null,
            IEnumerable<float> sensitivities = null)
        {
            return new Porcupine(accessKey, modelPath, keywordPaths, sensitivities);
        }

        /// <summary>
        /// Creates an instance of the Porcupine wake word engine.
        /// </summary>
        /// <param name="accessKey">AccessKey obtained from Picovoice Console (https://picovoice.ai/console/)</param>
        /// <param name="modelPath">Absolute path to file containing model parameters.</param>
        /// <param name="keywordPaths">A list of absolute paths to keyword model files.</param>
        /// <param name="sensitivities">
        /// A list of sensitivity values for each keyword. A higher sensitivity value lowers miss rate at the cost of increased 
        /// false alarm rate. A sensitivity value should be within [0, 1]. 
        /// </param>        
        private Porcupine(string accessKey, string modelPath, IEnumerable<string> keywordPaths, IEnumerable<float> sensitivities)
        {
            if (string.IsNullOrEmpty(accessKey))
            {
                throw new PorcupineInvalidArgumentException("No AccessKey provided to Porcupine");
            }

            modelPath = modelPath ?? DEFAULT_MODEL_PATH;
            if (!File.Exists(modelPath))
            {

#if !UNITY_EDITOR && UNITY_ANDROID

                try {
                    modelPath = ExtractResource(modelPath);
                } catch {
                    throw new PorcupineIOException($"Couldn't find model file at '{modelPath}'");
                }

#else

                throw new PorcupineIOException($"Couldn't find model file at '{modelPath}'");

#endif

            }

            if (keywordPaths == null || keywordPaths.Count() == 0)
            {
                throw new PorcupineInvalidArgumentException("No keyword file paths were provided to Porcupine");
            }

#if !UNITY_EDITOR && UNITY_ANDROID

            List<String> keywordList = keywordPaths.ToList();        
            for (int i = 0; i < keywordList.Count(); i++)
            {
                if (!File.Exists(keywordList[i]))
                {
                    try
                    {
                        keywordList[i] = ExtractResource(keywordList[i]);
                    } 
                    catch
                    {
                        throw new PorcupineIOException($"Couldn't find keyword file at '{keywordList[i]}'");
                    }
                }
            }
            keywordPaths = keywordList;

#else

            foreach (string path in keywordPaths)
            {
                if (!File.Exists(path))
                {
                    throw new PorcupineIOException($"Couldn't find keyword file at '{path}'");
                }
            }

#endif

            if (sensitivities == null)
            {
                sensitivities = Enumerable.Repeat(0.5f, keywordPaths.Count());
            }
            else
            {
                if (sensitivities.Any(s => s < 0 || s > 1))
                {
                    throw new PorcupineInvalidArgumentException("Sensitivities should be within [0, 1].");
                }
            }

            if (sensitivities.Count() != keywordPaths.Count())
            {
                throw new PorcupineInvalidArgumentException($"Number of keywords ({keywordPaths.Count()}) does not match number of sensitivities ({sensitivities.Count()})");
            }

            PorcupineStatus status = pv_porcupine_init(
                accessKey,
                modelPath,
                keywordPaths.Count(),
                keywordPaths.ToArray(),
                sensitivities.ToArray(),
                out _libraryPointer);
            if (status != PorcupineStatus.SUCCESS)
            {
                throw PorcupineStatusToException(status, "Porcupine init failed.");
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
                throw new PorcupineInvalidArgumentException(
                    $"Input audio frame size ({pcm.Length}) was not the size specified by Porcupine engine ({FrameLength}). " +
                    $"Use Porcupine.FrameLength to get the correct size.");
            }

            int keywordIndex;
            PorcupineStatus status = pv_porcupine_process(_libraryPointer, pcm, out keywordIndex);
            if (status != PorcupineStatus.SUCCESS)
            {
                throw PorcupineStatusToException(status, "Porcupine process failed.");
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
        /// <returns>Version of Porcupine</returns>
        public string Version { get; private set; }


        /// <summary>
        /// Coverts status codes to relevant .NET exceptions
        /// </summary>
        /// <param name="status">Picovoice library status code.</param>
        /// <returns>.NET exception</returns>
        private static PorcupineException PorcupineStatusToException(
            PorcupineStatus status,
            string message = "")
        {
            switch (status)
            {
                case PorcupineStatus.OUT_OF_MEMORY:
                    return new PorcupineMemoryException(message);
                case PorcupineStatus.IO_ERROR:
                    return new PorcupineIOException(message);
                case PorcupineStatus.INVALID_ARGUMENT:
                    return new PorcupineInvalidArgumentException(message);
                case PorcupineStatus.STOP_ITERATION:
                    return new PorcupineStopIterationException(message);
                case PorcupineStatus.KEY_ERROR:
                    return new PorcupineKeyException(message);
                case PorcupineStatus.INVALID_STATE:
                    return new PorcupineInvalidStateException(message);
                case PorcupineStatus.RUNTIME_ERROR:
                    return new PorcupineRuntimeException(message);
                case PorcupineStatus.ACTIVATION_ERROR:
                    return new PorcupineActivationException(message);
                case PorcupineStatus.ACTIVATION_LIMIT_REACHED:
                    return new PorcupineActivationLimitException(message);
                case PorcupineStatus.ACTIVATION_THROTTLED:
                    return new PorcupineActivationThrottledException(message);
                case PorcupineStatus.ACTIVATION_REFUSED:
                    return new PorcupineActivationRefusedException(message);
                default:
                    return new PorcupineException("Unmapped error code returned from Porcupine.");
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
                    throw new PorcupineRuntimeException(string.Format("Platform '{0}' not supported by Porcupine Unity binding", Application.platform));
            }
        }

        private static string GetDefaultModelPath()
        {

#if !UNITY_EDITOR && UNITY_ANDROID

            return ExtractResource(Path.Combine(Application.streamingAssetsPath, "porcupine_params.pv"));

#else

            return Path.Combine(Application.streamingAssetsPath, "porcupine_params.pv");

#endif

        }

        private static Dictionary<BuiltInKeyword, string> GetBuiltInKeywordPaths(string platform)
        {

#if !UNITY_EDITOR && UNITY_ANDROID

            string keywordFilesDir = Path.Combine(Path.Combine(Application.persistentDataPath, "keyword_files"), platform);
            if (!Directory.Exists(keywordFilesDir))
            {
                Directory.CreateDirectory(keywordFilesDir);
            }

            string assetDir = Path.Combine(Path.Combine(Application.streamingAssetsPath, "keyword_files"), platform);
            foreach (string keyword in Enum.GetNames(typeof(BuiltInKeyword))) 
            {
                ExtractResource(Path.Combine(
                    assetDir,
                    string.Format("{0}_{1}.ppn", keyword.Replace("_", " ").ToLower(), platform)));
            }            

#else

            string keywordFilesDir = Path.Combine(Application.streamingAssetsPath, "keyword_files", platform);

#endif

            Dictionary<BuiltInKeyword, string> keywordPaths = new Dictionary<BuiltInKeyword, string>();
            foreach (string keywordFile in Directory.GetFiles(keywordFilesDir))
            {
                if (Path.GetFileName(keywordFile).EndsWith(".meta"))
                {
                    continue;
                }
                string enumName = Path.GetFileName(keywordFile).Split('_')[0].Replace(" ", "_").ToUpper();
                if (!Enum.IsDefined(typeof(BuiltInKeyword), enumName))
                {
                    continue;
                }
                BuiltInKeyword builtin = (BuiltInKeyword)Enum.Parse(typeof(BuiltInKeyword), enumName);
                keywordPaths.Add(builtin, Path.Combine(keywordFilesDir, keywordFile));
            }

            return keywordPaths;
        }

#if !UNITY_EDITOR && UNITY_ANDROID

        public static string ExtractResource(string filePath)
        {
            if (!filePath.StartsWith(Application.streamingAssetsPath))
            {
                throw new PorcupineIOException($"File '{filePath}' not found in streaming assets path.");
            }

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
        }
#endif

    }
}