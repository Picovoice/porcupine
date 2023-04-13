/*
    Copyright 2020-2023 Picovoice Inc.

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
using System.Reflection;
using System.Runtime.InteropServices;


namespace Pv
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

    /// <summary>
    ///  .NET binding for Porcupine wake word engine.
    /// </summary>
    public class Porcupine : IDisposable
    {
        private const string LIBRARY = "libpv_porcupine";
        private IntPtr _libraryPointer = IntPtr.Zero;

        public static readonly string DEFAULT_MODEL_PATH;
        public static readonly Dictionary<BuiltInKeyword, string> BUILT_IN_KEYWORD_PATHS;

        static Porcupine()
        {

#if NETCOREAPP3_0_OR_GREATER

            NativeLibrary.SetDllImportResolver(typeof(Porcupine).Assembly, ImportResolver);

#endif

            DEFAULT_MODEL_PATH = Utils.PvModelPath();
            BUILT_IN_KEYWORD_PATHS = Utils.PvKeywordPaths();
        }

#if NETCOREAPP3_0_OR_GREATER

        private static IntPtr ImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
        {

#pragma warning disable IDE0058
#pragma warning disable IDE0059

            IntPtr libHandle = IntPtr.Zero;
            NativeLibrary.TryLoad(Utils.PvLibraryPath(libraryName), out libHandle);
            return libHandle;
        }

#pragma warning restore IDE0059
#pragma warning restore IDE0058

#endif

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PorcupineStatus pv_porcupine_init(
            IntPtr accessKey,
            IntPtr modelPath,
            int numKeywords,
            out IntPtr keywordPaths,
            float[] sensitivities,
            out IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern int pv_sample_rate();

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern void pv_porcupine_delete(IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PorcupineStatus pv_porcupine_process(
            IntPtr handle,
            short[] pcm,
            out int keywordIndex);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr pv_porcupine_version();

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern int pv_porcupine_frame_length();

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
                .Where(k => BUILT_IN_KEYWORD_PATHS.ContainsKey(k))
                .Select(k => BUILT_IN_KEYWORD_PATHS[k]);

            return new Porcupine(accessKey, keywordPaths, modelPath, sensitivities);
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
            return new Porcupine(accessKey, keywordPaths, modelPath, sensitivities);
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
        private Porcupine(
            string accessKey,
            IEnumerable<string> keywordPaths,
            string modelPath = null,
            IEnumerable<float> sensitivities = null)
        {
            if (string.IsNullOrEmpty(accessKey))
            {
                throw new PorcupineInvalidArgumentException("No AccessKey provided to Porcupine");
            }

            modelPath = modelPath ?? DEFAULT_MODEL_PATH;
            if (!File.Exists(modelPath))
            {
                throw new PorcupineIOException($"Couldn't find model file at '{modelPath}'");
            }

            if (keywordPaths == null || keywordPaths.Count() == 0)
            {
                throw new PorcupineInvalidArgumentException("No keyword file paths were provided to Porcupine");
            }

            foreach (string path in keywordPaths)
            {
                if (!File.Exists(path))
                {
                    throw new PorcupineIOException($"Couldn't find keyword file at '{path}'");
                }
            }

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

            IntPtr accessKeyPtr = Utils.GetPtrFromUtf8String(accessKey);
            IntPtr modelPathPtr = Utils.GetPtrFromUtf8String(modelPath);

            string[] keywordPathsArray = keywordPaths.ToArray();
            IntPtr[] keywordPathsPtr = new IntPtr[keywordPathsArray.Length];
            for (int i = 0; i < keywordPathsArray.Length; i++)
            {
                keywordPathsPtr[i] = Utils.GetPtrFromUtf8String(keywordPathsArray[i]);
            }

            PorcupineStatus status = pv_porcupine_init(
                accessKeyPtr,
                modelPathPtr,
                keywordPathsPtr.Length,
                out keywordPathsPtr[0],
                sensitivities.ToArray(),
                out _libraryPointer);

            Marshal.FreeHGlobal(accessKeyPtr);
            Marshal.FreeHGlobal(modelPathPtr);
            for (int i = 0; i < keywordPathsPtr.Length; i++)
            {
                Marshal.FreeHGlobal(keywordPathsPtr[i]);
            }

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
    }
}