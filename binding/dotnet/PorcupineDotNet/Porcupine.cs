/*
    Copyright 2018-2020 Picovoice Inc.

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
using System.Runtime.InteropServices;

namespace PorcupineDotNet
{
    /// <summary>
    ///  Status codes returned by Picovoice library
    /// </summary>
    public enum PvStatus
    {
        SUCCESS = 0,
        OUT_OF_MEMORY = 1,
        IO_ERROR = 2,
        INVALID_ARGUMENT = 3,
        STOP_ITERATION = 4,
        KEY_ERROR = 5,
        INVALID_STATE = 6,
    }

    /// <summary>
    ///  .NET Core binding for Picovoice's wake word detection engine (Porcupine).
    /// </summary>
    public class Porcupine
    {
        private const string PORCUPINE_LIBRARY = "libpv_porcupine";
        private IntPtr _libraryPointer = IntPtr.Zero;

        [DllImport(PORCUPINE_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PvStatus pv_porcupine_init(string modelPath, int numKeywords, string[] keywordPaths, float[] sensitivities, out IntPtr handle);

        [DllImport(PORCUPINE_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_sample_rate();

        [DllImport(PORCUPINE_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void pv_porcupine_delete(IntPtr handle);

        [DllImport(PORCUPINE_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PvStatus pv_porcupine_process(IntPtr handle, short[] pcm, out int keywordIndex);

        [DllImport(PORCUPINE_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr pv_porcupine_version();

        [DllImport(PORCUPINE_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_porcupine_frame_length();

        /// <summary>
        /// Creates an instance of the Porcupine wake word engine.
        /// </summary>
        /// <param name="modelPath">Absolute path to file containing model parameters.</param>
        /// <param name="keywordPaths">A list of absolute paths to keyword model files.</param>
        /// <param name="sensitivities">A list of sensitivity values for each keyword. 
        ///     A higher sensitivity value lowers miss rate at the cost of increased false alarm rate.
        ///     A sensitivity value should be within[0, 1]. </param>        
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

            foreach (string p in keywordPaths)
            {
                if (!File.Exists(p))
                {
                    throw new IOException($"Couldn't find keyword file at '{p}'");
                }
            }

            if (sensitivities.Any(s => s < 0 || s > 1))
            {
                throw new ArgumentException("Sensitivies should be within [0, 1].");
            }

            if (keywordPaths.Count() != sensitivities.Count())
            {
                throw new ArgumentException("Each given keyword needs a valid sensitivity value.");
            }

            PvStatus status = pv_porcupine_init(modelPath, keywordPaths.Count(), keywordPaths.ToArray(), sensitivities.ToArray(), out _libraryPointer);
            if (status != PvStatus.SUCCESS)
            {
                throw PvStatusToException(status);
            }
        }

        /// <summary>
        /// Process a frame of audio with the wake word engine.
        /// </summary>
        /// <param name="data">
        /// A frame of audio samples to be assessed by Porcupine. 
        /// The required audio format is found by calling pv_sample_rate() to get the required sample rate and pv_porcupine_frame_length() to get the required frame size.
        /// Audio must be single-channel and 16-bit linearly-encoded. 
        /// </param>
        /// <returns>
        /// Index of detected keyword, if any. Indexing is 0-based and according to ordering of input keyword file paths. 
        /// Returns -1 if no detection is made.
        /// </returns>
        public int Process(short[] data)
        {
            if (data.Length != pv_porcupine_frame_length())
            {
                throw new ArgumentException("Input audio frame was not the size specified by Porcupine engine. Use Porcupine.FrameLength.");
            }

            PvStatus status = pv_porcupine_process(_libraryPointer, data, out int index);
            if (status != PvStatus.SUCCESS)
            {
                throw PvStatusToException(status);
            }

            return index;
        }

        /// <summary>
        /// Get the audio sample rate required by Porcupine.
        /// </summary>
        /// <returns>Required sample rate.</returns>
        public int SampleRate => pv_sample_rate();

        /// <summary>
        /// Gets the required number of audio samples per frame.
        /// </summary>
        /// <returns>Required frame length.</returns>
        public int FrameLength => pv_porcupine_frame_length();

        /// <summary>
        /// Gets the version number of the Porcupine library.
        /// </summary>
        /// <returns>Version of Pocupine</returns>
        public string Version => Marshal.PtrToStringAnsi(pv_porcupine_version());


        /// <summary>
        /// Coverts status codes to relavent .NET exceptions
        /// </summary>
        /// <param name="status">Picovoice library status code.</param>
        /// <returns>.NET exception</returns>
        private static Exception PvStatusToException(PvStatus status)
        {
            switch (status)
            {
                case PvStatus.OUT_OF_MEMORY:
                    return new OutOfMemoryException("Porcupine library ran out of memory.");
                case PvStatus.IO_ERROR:
                    return new IOException("Porcupine library reported an I/O error.");
                case PvStatus.INVALID_ARGUMENT:
                    return new ArgumentException("An invalid argument was reported by the Porcupine library.");
                case PvStatus.STOP_ITERATION:
                    return new Exception("A stop iteration status was returned by the Porcupine library");
                case PvStatus.KEY_ERROR:
                    return new Exception("A key error was returned by the Porcupine library");
                case PvStatus.INVALID_STATE:
                    return new Exception("The Porcupine library reported an invalid state.");
                default:
                    return new Exception();
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
            }
            GC.Collect();
        }

        ~Porcupine()
        {
            Dispose();
        }
    }
}
