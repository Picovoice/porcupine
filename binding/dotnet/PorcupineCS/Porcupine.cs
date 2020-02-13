using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

namespace PorcupineCS
{
    public enum Status
    {
        SUCCESS = 0,
        OUT_OF_MEMORY,
        IO_ERROR,
        INVALID_ARGUMENT,
        STOP_ITERATION,
        KEY_ERROR,
        INVALID_STATE,
    }

    public class Porcupine
    {
        private const string LIBRARY_NAME = "libpv_porcupine";
        private IntPtr _libraryPointer;
        private static readonly string _extension = $"{GetExtension()}";

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern Status pv_porcupine_init(string modelPath, int numKeywords, string[] keywordPaths, float[] sensitivities, out IntPtr handle);

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_sample_rate();

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void pv_porcupine_delete(IntPtr handle);

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern Status pv_porcupine_process(IntPtr handle, short[] pcm, out int keywordIndex);

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern string pv_porcupine_version();

        [DllImport(LIBRARY_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int pv_porcupine_frame_length();

        public Porcupine(string modelPath, IEnumerable<string> keywordPaths, IEnumerable<float> sensitivities)
        {
            if(!File.Exists(LIBRARY_NAME + _extension)) {
                Console.WriteLine(LIBRARY_NAME + _extension);
                throw new Exception($"the {LIBRARY_NAME} cannot be found.\nThis should be in the same folder as this or on a known path.");
            }
            if (keywordPaths.Count() != sensitivities.Count())
            {
                throw new ArgumentException();
            }

            Console.WriteLine(keywordPaths.Count());
            Status status = pv_porcupine_init(modelPath, keywordPaths.Count(), keywordPaths.ToArray(), sensitivities.ToArray(), out _libraryPointer);
            if (status != Status.SUCCESS) {
                throw new Exception();
            }
        }

        public Int32 Process(Int16[] data)
        {
            if (data.Count() != pv_porcupine_frame_length()) {
                throw new ArgumentException();
            }
            
            Int32 index;
            Status status = pv_porcupine_process(_libraryPointer, data, out index);
            if (status != Status.SUCCESS) {
                throw new Exception();
            }

            return index;
        }

        public int SampleRate()
        {
            return pv_sample_rate();
        }

        public int FrameLength()
        {
            return pv_porcupine_frame_length();
        }

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
