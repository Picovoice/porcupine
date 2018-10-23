using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

namespace PorcupineCS
{

    // ReSharper disable UnusedMember.Global
    // ReSharper disable IdentifierTypo
    // ReSharper disable InconsistentNaming
    public enum PicoVoiceStatus
    {
        SUCCESS = 0,
        OUTOFMEMORY = 1,
        IOERROR = 2,
        INVALIDARGUMENT = 3
    }
    // ReSharper restore InconsistentNaming
    // ReSharper restore IdentifierTypo
    // ReSharper restore UnusedMember.Global

    public class Porcupine
    {
        public PicoVoiceStatus Status { get; private set; }
        private const string LIBRARY_NAME = "libpv_porcupine";
        private IntPtr _libraryPointer;
        private static readonly string _extension = $"{GetExtension()}";

        #region PINVOKE

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PicoVoiceStatus pv_porcupine_init(string modelFilepath, string keywordsFilePath, float sensitivity, out IntPtr pointer);

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PicoVoiceStatus pv_porcupine_multiple_keywords_init(string modelFilepath, int numberOfKeywords, string[] keywordsFilePaths, float[] sensitivities, out IntPtr pointer);

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_sample_rate();

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void pv_porcupine_delete(IntPtr pointer);

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PicoVoiceStatus pv_porcupine_process(IntPtr pointer, short[] voiceData, out bool results);

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern PicoVoiceStatus pv_porcupine_multiple_keywords_process(IntPtr pointer, short[] voiceData, out int keywordIndex);

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern string pv_porcupine_version();

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_porcupine_frame_length();

        #endregion

        /// <summary>
        /// Loads Porcupine's shared library and creates an instance of wake word detection object.
        /// </summary>
        /// <param name="libraryPath">Absolute path to Porcupine's shared library.</param>
        /// <param name="modelFilePath">Absolute path to file containing model parameters.</param>
        /// <param name="keywordFilePath">Absolute path to keyword file containing hyper-parameters. If not present then 'keyword_file_paths' will be used.</param>
        /// <param name="sensitivity">Sensitivity parameter. A higher sensitivity value lowers miss rate at the cost of increased false alarm rate.
        /// For more information regarding this parameter refer to 'include/pv_porcupine.h'.
        /// If not present then 'sensitivities' is used.</param>
        /// <param name="keywordFilePaths"> List of absolute paths to keyword files. Intended to be used for multiple keyword scenario.
        /// This parameter is used only when 'keyword_file_path' is not set.</param>
        /// <param name="sensitivities"> List of sensitivity parameters. Intended to be used for multiple keyword scenario.
        /// This parameter is used only when 'sensitivity' is not set.</param>
        public Porcupine(string modelFilePath, string keywordFilePath = null,
            float? sensitivity = null, IEnumerable<string> keywordFilePaths = null,
            IEnumerable<float> sensitivities = null)
        {
            if(!File.Exists(LIBRARY_NAME + _extension))
                throw new Exception($"the {LIBRARY_NAME} cannot be found.\nThis should be in the same folder as this or on a known path.");
            if (keywordFilePath == null)
            {
                if(keywordFilePaths == null)
                    throw new ArgumentNullException(nameof(keywordFilePaths));

                if(sensitivities == null)
                    throw new ArgumentNullException(nameof(sensitivities));

                Status = pv_porcupine_multiple_keywords_init(modelFilePath, keywordFilePaths.Count(), keywordFilePaths.ToArray(), sensitivities.ToArray(), out _libraryPointer);
            }
            else
            {
                if (sensitivity == null)
                    throw new ArgumentNullException(nameof(sensitivity));

                Status = pv_porcupine_init(modelFilePath, keywordFilePath, sensitivity.Value, out _libraryPointer);
            }
        }

        /// <summary>
        /// Monitors incoming audio stream for a given keyword.
        /// </summary>
        /// <param name="data">A frame of audio samples. The number of samples per frame can be attained by calling 'pv_porcupine_frame_length()'.
        /// The incoming audio needs to have a sample rate equal to 'pv_sample_rate()' and be 16-bit linearly-encoded.
        /// Furthermore, porcupine operates on single channel audio.</param>
        /// <param name="result">result Flag indicating if the keyword has been observed ending at the current frame.</param>
        /// <returns>Status code. Returns 'PV_STATUS_INVALID_ARGUMENT' on failure.</returns>
        public PicoVoiceStatus Process(Int16[] data, out bool result)
        {
            return pv_porcupine_process(_libraryPointer, data, out result);
        }

        /// <summary>
        /// Monitors incoming audio stream for multiple keywords.
        /// </summary>
        /// <param name="data">A frame of audio samples. For more information about required audio properties refer to documentation of '<seealso cref="Process"/>'.</param>
        /// <param name="index">Index of observed keyword at the end of current frame.
        /// Indexing is 0-based and based on the ordering of 'keyword_file_paths' passed to 'pv_porcupine_multiple_keywords_init()'.
        /// If no keyword is detected it is set to -1.</param>
        /// <returns>Status code. Returns 'PV_STATUS_INVALID_ARGUMENT' on failure.</returns>
        public PicoVoiceStatus ProcessMultipleKeywords(Int16[] data, out int index)
        {
            return pv_porcupine_multiple_keywords_process(_libraryPointer, data, out index);
        }

        /// <summary>
        /// Audio sample rate accepted by Picovoice.
        /// </summary>
        /// <returns></returns>
        public int SampleRate()
        {
            return pv_sample_rate();
        }

        /// <summary>
        /// Getter for length (number of audio samples) per frame.
        /// </summary>
        /// <returns></returns>
        public int FrameLength()
        {
            return pv_porcupine_frame_length();
        }

        /// <summary>
        /// Getter for the version string of the library
        /// </summary>
        /// <returns></returns>
        public string GetVersion()
        {
            return pv_porcupine_version();
        }

        private static string GetExtension()
        {
            PlatformID platform = Environment.OSVersion.Platform;
            if (platform == PlatformID.MacOSX)
            {
                return ".dylib";
            }

            if (platform == PlatformID.Unix)
            {
                return ".so";
            }

            if (platform == PlatformID.Win32NT)
            {
                return ".dll";
            }

            throw new NotImplementedException("this OS has no binding logic implemented yet.");
        }

        ~Porcupine()
        {
            pv_porcupine_delete(_libraryPointer);
        }

        public void Dispose()
        {
            pv_porcupine_delete(_libraryPointer);
            _libraryPointer = IntPtr.Zero;
        }
    }
}
