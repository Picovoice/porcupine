
using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
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
        private const string LIBRARY_NAME = "libpv_porcupine";
        private IntPtr _libraryPointer;
        private static readonly string _extension = $"{(Environment.OSVersion.Platform == PlatformID.Unix ? ".so" : "dll")}";
        private string _modelFilePath;
        private string _keywordFilePath;
        private double? _sensitivity;
        private IEnumerable<string> _keywordFilePaths;
        private IEnumerable<double> _sensitivities;

        #region PINVOKE

        [DllImport(LIBRARY_NAME, PreserveSig = true, CallingConvention = CallingConvention.Cdecl)]
        private static extern PicoVoiceStatus pv_porcupine_init(string modelFilepath, string keywordsFilePath, float sensitivity, out IntPtr pointer);


        [DllImport(LIBRARY_NAME, PreserveSig = true, CallingConvention = CallingConvention.Cdecl)]
        private static extern PicoVoiceStatus pv_porcupine_multiple_keywords_init(string modelFilepath, string[] keywordsFilePaths, float[] sensitivities, out IntPtr pointer);

        [DllImport(LIBRARY_NAME, PreserveSig = true, CallingConvention = CallingConvention.Cdecl)]
        private static extern int pv_sample_rate();

        [DllImport(LIBRARY_NAME, PreserveSig = true, CallingConvention = CallingConvention.Cdecl)]
        private static extern void pv_porcupine_delete(IntPtr pointer);

        [DllImport(LIBRARY_NAME, PreserveSig = true, CallingConvention = CallingConvention.Cdecl)]
        private static extern PicoVoiceStatus pv_porcupine_process(IntPtr pointer, short[] voiceData, out bool results);

        [DllImport(LIBRARY_NAME, PreserveSig = true, CallingConvention = CallingConvention.Cdecl)]
        private static extern PicoVoiceStatus pv_porcupine_multiple_keywords_process(IntPtr pointer, short[] voiceData, int keywordIndex);

        [DllImport(LIBRARY_NAME, PreserveSig = true, CallingConvention = CallingConvention.Cdecl)]
        private static extern string pv_porcupine_version();

        [DllImport(LIBRARY_NAME, PreserveSig = true, CallingConvention = CallingConvention.Cdecl)]
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
            double? sensitivity = null, IEnumerable<string> keywordFilePaths = null,
            IEnumerable<double> sensitivities = null)
        {
            _modelFilePath = modelFilePath;
            _sensitivities = sensitivities;
            _keywordFilePaths = keywordFilePaths;
            _sensitivity = sensitivity;
            _keywordFilePath = keywordFilePath;
            if(!File.Exists(LIBRARY_NAME + _extension))
                throw new Exception($"the {LIBRARY_NAME} cannot be found.\nThis should be in the same folder as this or on a known path.");
        }


        ~Porcupine()
        {
            pv_porcupine_delete(_libraryPointer);
        }
    }
}
