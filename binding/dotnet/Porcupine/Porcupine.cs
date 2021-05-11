/*
    Copyright 2020-2021 Picovoice Inc.

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
    ///  Status codes returned by Picovoice library
    /// </summary>
    public enum PorcupineStatus
    {
        SUCCESS = 0,
        OUT_OF_MEMORY = 1,
        IO_ERROR = 2,
        INVALID_ARGUMENT = 3
    }

    /// <summary>
    ///  .NET binding for Porcupine wake word engine.
    /// </summary>
    public class Porcupine : IDisposable
    {
        private const string LIBRARY = "libpv_porcupine";
        private IntPtr _libraryPointer = IntPtr.Zero;
        
        public static string MODEL_PATH;
        public static Dictionary<string, string> KEYWORD_PATHS;
        public static List<string> KEYWORDS;
      
        static Porcupine()
        {            
#if NETCOREAPP3_1
            NativeLibrary.SetDllImportResolver(typeof(Porcupine).Assembly, ImportResolver);
#endif
            MODEL_PATH = Utils.PvModelPath();
            KEYWORD_PATHS = Utils.PvKeywordPaths();
            KEYWORDS = KEYWORD_PATHS.Keys.ToList();
        }

#if NETCOREAPP3_1
        private static IntPtr ImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath) 
        {
            IntPtr libHandle = IntPtr.Zero;                                   
            NativeLibrary.TryLoad(Utils.PvLibraryPath(libraryName), out libHandle);
            return libHandle;
        }
#endif

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PorcupineStatus pv_porcupine_init(string modelPath, int numKeywords, string[] keywordPaths, float[] sensitivities, out IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_sample_rate();

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void pv_porcupine_delete(IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PorcupineStatus pv_porcupine_process(IntPtr handle, short[] pcm, out int keywordIndex);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr pv_porcupine_version();

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_porcupine_frame_length();
        /// <summary>
        /// Creates an instance of the Porcupine wake word engine.
        /// </summary>
        /// <param name="modelPath">Absolute path to the file containing model parameters. If not set it will be set to the default location.</param>
        /// <param name="keywordPaths"> Absolute paths to keyword model files. If not set it will be populated from `keywords` argument.</param>
        /// <param name="keywords">
        /// List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved 
        /// using `Porcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
        /// </param>
        /// <param name="sensitivities">
        /// Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer 
        /// misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.
        /// </param>
        /// <returns>An instance of Porcupine wake word engine.</returns>                             
        public static Porcupine Create(string modelPath = null, IEnumerable<string> keywordPaths = null,
                                       IEnumerable<string> keywords = null, IEnumerable<float> sensitivities = null)
        {
            modelPath = modelPath ?? MODEL_PATH;

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

                if (keywords.All(k => KEYWORDS.Contains(k)))
                {                    
                    keywordPaths = keywords.Select(k => KEYWORD_PATHS[k]);
                }
                else
                {
                    throw new ArgumentException("One or more keywords are not available by default. Available default keywords are:\n" +
                                                 string.Join(",", KEYWORDS));
                }
            }

            if (sensitivities == null)
            {
                sensitivities = Enumerable.Repeat(0.5f, keywordPaths.Count());
            }

            if (sensitivities.Count() != keywordPaths.Count())
            {
                throw new ArgumentException($"Number of keywords ({keywordPaths.Count()}) does not match number of sensitivities ({sensitivities.Count()})");
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
        public Porcupine(string modelPath, IEnumerable<string> keywordPaths, IEnumerable<float> sensitivities)
        {
            if (string.IsNullOrWhiteSpace(modelPath) ||
                keywordPaths == null || keywordPaths.Count() == 0 ||
                sensitivities == null || sensitivities.Count() == 0)
            {
                throw new ArgumentNullException("One or more of the constructor arguments were null or empty. ");
            }

            if (!File.Exists(modelPath))
            {
                throw new IOException($"Couldn't find model file at '{modelPath}'");
            }

            foreach (string path in keywordPaths)
            {
                if (!File.Exists(path))
                {
                    throw new IOException($"Couldn't find keyword file at '{path}'");
                }
            }

            if (sensitivities.Any(s => s < 0 || s > 1))
            {
                throw new ArgumentException("Sensitivities should be within [0, 1].");
            }

            if (keywordPaths.Count() != sensitivities.Count())
            {
                throw new ArgumentException($"Number of keywords ({keywordPaths.Count()}) does not match number of sensitivities ({sensitivities.Count()})");
            }

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
                throw new ArgumentException($"Input audio frame size ({pcm.Length}) was not the size specified by Porcupine engine ({FrameLength}). " +
                    $"Use Porcupine.FrameLength to get the correct size.");
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
                    return new Exception($"Unmapped error code returned from Porcupine.");
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
