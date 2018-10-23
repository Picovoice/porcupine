/* This file contains the WAVFile class and related things.  The WAVFile class
 * allows manipulation of WAV audio files.  I used the following web pages as
 * resources on the WAV file format:
 * http://ccrma.stanford.edu/courses/422/projects/WaveFormat
 * http://www.sonicspot.com/guide/wavefiles.html
 * http://technology.niagarac.on.ca/courses/ctec1631/WavFileFormat.html
 * http://www.ringthis.com/dev/wave_format.htm
 *
 * Date       Author         Description
 * 2009-03-09 erico          Created
 * 2009-03-10 erico          Continued work
 * 2009-03-12 erico          Continued work
 * 2009-03-13 erico          Continued work
 * 2009-03-14 erico          Updated Open() to correct mDataSizeBytes if it is
 *                           not what it should be (it should be mFileSizeBytes - 36).
 *                           Worked on updating the comments with comment tags.
 * 2009-03-16 erico          Added MergeAudioFiles().
 * 2009-03-17 erico          Updated to use the new exception classes added to WAVFileExceptions.cs.
 *                           Also, removed (commented out) the mFileSizeBytes and mDataBytesWritten
 *                           members because we can calculate those values using mFileStream.
 * 2009-03-18 erico          Fixed a bug in writing the WAV file size in the header (related to having
 *                           removed mDataBytesWritten).
 * 2009-03-19 erico          Added HighestBitsPerSample().  Replaced a couple functions with
 *                           HighestSampleValueAs16Bit(), which returns the highest sample value as
 *                           a 16-bit value, regardless of whether the file contains 8-bit or 16-bit
 *                           audio.  Also, added GetNextSampleAs16Bit().
 * 2009-03-20 erico          Added HighestNumChannels() and CopyAndConvert().
 */

using System;
using System.IO;
// ReSharper disable InconsistentNaming
// ReSharper disable MemberCanBePrivate.Global
// ReSharper disable FunctionComplexityOverflow

namespace PorcupineCS
{
    /// <summary>
    /// WAV file class: Allows manipulation of WAV audio files
    /// </summary>
    public class WAVFile
    {
        /// <summary>
        /// This enumeration specifies file modes supported by the
        /// class.
        /// </summary>
        public enum WAVFileMode
        {
            READ,
            WRITE,
            READ_WRITE
        }

        /// <summary>
        /// WAVFile class: Static constructor
        /// </summary>
        static WAVFile()
        {
            mDataStartPos = 44;
        }

        /// <summary>
        /// WAVFile class: Default constructor
        /// </summary>
        public WAVFile()
        {
            InitMembers();
        }

        /// <summary>
        /// Destructor - Makes sure the file is closed.
        /// </summary>
        ~WAVFile()
        {
            // Make sure the file is closed
            Close();
        }

        /// <summary>
        /// Opens a WAV file and attemps to read the header & audio information.
        /// </summary>
        /// <param name="pFilename">The name of the file to open</param>
        /// <param name="pMode">The file opening mode.  Only READ and READ_WRITE are valid.  If you want to write only, then use Create().</param>
        /// <returns>A blank string on success, or a message on failure.</returns>
        public string Open(string pFilename, WAVFileMode pMode)
        {
            mFilename = pFilename;
            return (Open(pMode));
        }

        /// <summary>
        /// Opens the file specified by mFilename and reads the file header and audio information.  Does not read any of the audio data.
        /// </summary>
        /// /// <param name="pMode">The file opening mode.  Only READ and READ_WRITE are valid.  If you want to write only, then use Create().</param>
        /// <returns>A blank string on success, or a message on failure.</returns>
        public string Open(WAVFileMode pMode)
        {
            if (mFileStream != null)
            {
                mFileStream.Close();
                mFileStream.Dispose();
                mFileStream = null;
            }
            string filenameBackup = mFilename;
            InitMembers();

            // pMode should be READ or READ_WRITE.  Otherwise, throw an exception.  For
            // write-only mode, the user can call Create().
            if ((pMode != WAVFileMode.READ) && (pMode != WAVFileMode.READ_WRITE))
                throw new WAVFileException("File mode not supported: " + WAVFileModeStr(pMode), "WAVFile.Open()");

            if (!File.Exists(filenameBackup))
                return ("File does not exist: " + filenameBackup);

            if (!IsWaveFile(filenameBackup))
                return ("File is not a WAV file: " + filenameBackup);

            mFilename = filenameBackup;

            string retval = "";

            try
            {
                mFileStream = File.Open(mFilename, FileMode.Open);
                mFileMode = pMode;

                // RIFF chunk (12 bytes total)
                // Read the header (first 4 bytes)
                byte[] buffer = new byte[4];
                mFileStream.Read(buffer, 0, 4);
                buffer.CopyTo(mWAVHeader, 0);
                // Read the file size (4 bytes)
                mFileStream.Read(buffer, 0, 4);
                //mFileSizeBytes = BitConverter.ToInt32(buffer, 0);
                // Read the RIFF type
                mFileStream.Read(buffer, 0, 4);
                buffer.CopyTo(mRIFFType, 0);

                // Format chunk (24 bytes total)
                // "fmt " (ASCII characters)
                mFileStream.Read(buffer, 0, 4);
                // Length of format chunk (always 16)
                mFileStream.Read(buffer, 0, 4);
                // 2 bytes (value always 1)
                mFileStream.Read(buffer, 0, 2);
                // # of channels (2 bytes)
                mFileStream.Read(buffer, 2, 2);
                mNumChannels = (BitConverter.IsLittleEndian ? buffer[2] : buffer[3]);
                // Sample rate (4 bytes)
                mFileStream.Read(buffer, 0, 4);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer);
                mSampleRateHz = BitConverter.ToInt32(buffer, 0);
                // Bytes per second (4 bytes)
                mFileStream.Read(buffer, 0, 4);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer);
                mBytesPerSec = BitConverter.ToInt32(buffer, 0);
                // Bytes per sample (2 bytes)
                mFileStream.Read(buffer, 2, 2);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer, 2, 2);
                mBytesPerSample = BitConverter.ToInt16(buffer, 2);
                // Bits per sample (2 bytes)
                mFileStream.Read(buffer, 2, 2);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer, 2, 2);
                mBitsPerSample = BitConverter.ToInt16(buffer, 2);

                // Data chunk
                // "data" (ASCII characters)
                mFileStream.Read(buffer, 0, 4);
                // Length of data to follow (4 bytes)
                mFileStream.Read(buffer, 0, 4);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer);
                mDataSizeBytes = BitConverter.ToInt32(buffer, 0);

                // Total of 44 bytes read up to this point.

                // The data size should be file size - 36 bytes.  If not, then set
                // it to that.
                if (mDataSizeBytes != FileSizeBytes - 36)
                    mDataSizeBytes = (int)(FileSizeBytes - 36);

                // The rest of the file is the audio data, which
                // can be read by successive calls to NextSample().

                mNumSamplesRemaining = NumSamples;
            }
            catch (Exception exc)
            {
                retval = exc.Message;
            }

            return (retval);
        }

        /// <summary>
        /// Closes the file
        /// </summary>
        public void Close()
        {
            if (mFileStream != null)
            {
                // If in write or read/write mode, write the file size information to
                // the header.
                if ((mFileMode == WAVFileMode.WRITE) || (mFileMode == WAVFileMode.READ_WRITE))
                {
                    // File size: Offset 4, 4 bytes
                    mFileStream.Seek(4, 0);
                    // Note: Per the WAV file spec, we need to write file size - 8 bytes.
                    // The header is 44 bytes, and 44 - 8 = 36, so we write
                    // mDataBytesWritten + 36.
                    // 2009-03-17: Now using FileSizeBytes - 8 (to avoid mDataBytesWritten).
                    //mFileStream.Write(BitConverter.GetBytes(mDataBytesWritten+36), 0, 4);
                    int size = (int)FileSizeBytes - 8;
                    byte[] buffer = BitConverter.GetBytes(size);
                    if (!BitConverter.IsLittleEndian)
                        Array.Reverse(buffer);
                    mFileStream.Write(buffer, 0, 4);
                    // Data size: Offset 40, 4 bytes
                    mFileStream.Seek(40, 0);
                    //mFileStream.Write(BitConverter.GetBytes(mDataBytesWritten), 0, 4);
                    size = (int)(FileSizeBytes - mDataStartPos);
                    buffer = BitConverter.GetBytes(size);
                    if (!BitConverter.IsLittleEndian)
                        Array.Reverse(buffer);
                    mFileStream.Write(buffer, 0, 4);
                }

                mFileStream.Close();
                mFileStream.Dispose();
                mFileStream = null;
            }

            // Reset the members back to defaults
            InitMembers();
        }

        /// <summary>
        /// When in read mode, returns the next audio sample from the
        /// file.  The return value is a byte array and will contain one
        /// byte if the file contains 8-bit audio or 2 bytes if the file
        /// contains  16-bit audio.  The return value will be null if no
        /// next sample can be read.  For 16-bit samples, the byte array
        /// can be converted to a 16-bit integer using BitConverter.ToInt16().
        /// If there is an error, this method will throw a WAVFileReadException.
        /// </summary>
        /// <returns>A byte array containing the audio sample</returns>
        public byte[] GetNextSample_ByteArray()
        {
            byte[] audioSample;

            // Throw an exception if mFileStream is null
            if (mFileStream == null)
                throw new WAVFileReadException("Read attempted with null internal file stream.", "WAVFile.GetNextSample_ByteArray()");

            // We should be in read or read/write mode.
            if ((mFileMode != WAVFileMode.READ) && (mFileMode != WAVFileMode.READ_WRITE))
                throw new WAVFileReadException("Read attempted in incorrect mode: " + WAVFileModeStr(mFileMode), "WAVFile.GetNextSample_ByteArray()");

            try
            {
                int numBytes = mBitsPerSample / 8; // 8 bits per byte
                audioSample = new byte[numBytes];
                mFileStream.Read(audioSample, 0, numBytes);
                --mNumSamplesRemaining;
            }
            catch (Exception exc)
            {
                throw new WAVFileReadException(exc.Message, "WAVFile.GetNextSample_ByteArray()");
            }

            return audioSample;
        }

        /// <summary>
        /// When in read mode, returns the next audio sample from the loaded audio
        /// file.  This is a convenience method that can be used when it is known
        /// that the audio file contains 8-bit audio.  If the audio file is not
        /// 8-bit, this method will throw a WAVFileReadException. 
        /// </summary>
        /// <returns>The next audio sample from the loaded audio file.</returns>
        public byte GetNextSample_8bit()
        {
            // If the audio data is not 8-bit, throw an exception.
            if (mBitsPerSample != 8)
                throw new WAVFileReadException("Attempted to retrieve an 8-bit sample when audio is not 8-bit.", "WAVFile.GetNextSample_8bit()");

            byte sample8 = 0;

            // Get the next sample using GetNextSample_ByteArray()
            byte[] sample = GetNextSample_ByteArray();
            if (sample != null)
                sample8 = sample[0];

            return sample8;
        }

        /// <summary>
        /// When in read mode, returns the next audio sample from the loaded audio
        /// file.  This is a convenience method that can be used when it is known
        /// that the audio file contains 16-bit audio.  If the audio file is not
        /// 16-bit, this method will throw a WAVFileReadException. 
        /// </summary>
        /// <returns>The next audio sample from the loaded audio file.</returns>
        public short GetNextSample_16bit()
        {
            // If the audio data is not 16-bit, throw an exception.
            if (mBitsPerSample != 16)
                throw new WAVFileReadException("Attempted to retrieve a 16-bit sample when audio is not 16-bit.", "WAVFile.GetNextSample_16bit()");

            short sample16 = 0;

            // Get the next sample using GetNextSample_ByteArray()
            byte[] sample = GetNextSample_ByteArray();
            if (sample != null)
                sample16 = BitConverter.ToInt16(sample, 0);

            return sample16;
        }

        /// <summary>
        /// When in read mode, returns the next audio sample from the loaded audio
        /// file.  This returns the value as a 16-bit value regardless of whether the
        /// audio file is 8-bit or 16-bit.  If the audio is 8-bit, then the 8-bit sample
        /// value will be scaled to a 16-bit value.
        /// </summary>
        /// <returns>The next audio sample from the loaded audio file, as a 16-bit value, scaled if necessary.</returns>
        public short GetNextSampleAs16Bit()
        {
            short sample_16bit = 0;

            if (mBitsPerSample == 8)
                sample_16bit = ScaleByteToShort(GetNextSample_8bit());
            else if (mBitsPerSample == 16)
                sample_16bit = GetNextSample_16bit();

            return sample_16bit;
        }

        /// <summary>
        /// When in read mode, returns the next audio sample from the loaded audio
        /// file.  This returns the value as an 8-bit value regardless of whether the
        /// audio file is 8-bit or 16-bit.  If the audio is 16-bit, then the 16-bit sample
        /// value will be scaled to an 8-bit value.
        /// </summary>
        /// <returns>The next audio sample from the loaded audio file, as an 8-bit value, scaled if necessary.</returns>
        public byte GetNextSampleAs8Bit()
        {
            byte sample_8bit = 0;

            if (mBitsPerSample == 8)
                sample_8bit = GetNextSample_8bit();
            else if (mBitsPerSample == 16)
                sample_8bit = ScaleShortToByte(GetNextSample_16bit());

            return sample_8bit;
        }

        /// <summary>
        /// When in write mode, adds a new sample to the audio file.  Takes
        /// an array of bytes representing the sample.  The array should
        /// contain the correct number of bytes to match the sample size.
        /// If there are any errors, this method will throw a WAVFileWriteException.
        /// </summary>
        /// <param name="pSample">An array of bytes representing the audio sample</param>
        public void AddSample_ByteArray(byte[] pSample)
        {
            if (pSample != null)
            {
                // We should be in write or read/write mode.
                if ((mFileMode != WAVFileMode.WRITE) && (mFileMode != WAVFileMode.READ_WRITE))
                    throw new WAVFileWriteException("Write attempted in incorrect mode: " + WAVFileModeStr(mFileMode),
                                                    "WAVFile.AddSample_ByteArray()");

                // Throw an exception if mFileStream is null
                if (mFileStream == null)
                    throw new WAVFileWriteException("Write attempted with null internal file stream.", "WAVFile.AddSample_ByteArray()");

                // If pSample contains an incorrect number of bytes for the
                // sample size, then throw an exception.
                if (pSample.GetLength(0) != (mBitsPerSample / 8)) // 8 bits per byte
                    throw new WAVFileWriteException("Attempt to add an audio sample of incorrect size.", "WAVFile.AddSample_ByteArray()");

                try
                {
                    int numBytes = pSample.GetLength(0);
                    mFileStream.Write(pSample, 0, numBytes);
                    //mDataBytesWritten += numBytes;
                }
                catch (Exception exc)
                {
                    throw new WAVFileWriteException(exc.Message, "WAVFile.AddSample_ByteArray()");
                }
            }
        }

        /// <summary>
        /// When in write mode, adds an 8-bit sample to the audio file.
        /// Takes a byte containing the sample.  If the audio file is
        /// not 8-bit, this method will throw a WAVFileWriteException.
        /// </summary>
        /// <param name="pSample">The audio sample to add</param>
        public void AddSample_8bit(byte pSample)
        {
            // If the audio data is not 8-bit, throw an exception.
            if (mBitsPerSample != 8)
                throw new WAVFileWriteException("Attempted to add an 8-bit sample when audio file is not 8-bit.", "WAVFile.AddSample_8bit()");

            byte[] sample = new byte[1];
            sample[0] = pSample;
            AddSample_ByteArray(sample);
        }

        /// <summary>
        /// When in write mode, adds a 16-bit sample to the audio file.
        /// Takes an Int16 containing the sample.  If the audio file is
        /// not 16-bit, this method will throw a WAVFileWriteException.
        /// </summary>
        /// <param name="pSample">The audio sample to add</param>
        public void AddSample_16bit(short pSample)
        {
            // If the audio data is not 16-bit, throw an exception.
            if (mBitsPerSample != 16)
                throw new WAVFileWriteException("Attempted to add a 16-bit sample when audio file is not 16-bit.", "WAVFile.AddSample_16bit()");

            byte[] buffer = BitConverter.GetBytes(pSample);
            if (!BitConverter.IsLittleEndian)
                Array.Reverse(buffer);
            AddSample_ByteArray(buffer);
        }

        /// <summary>
        /// Creates a new WAV audio file.
        /// </summary>
        /// <param name="pFilename">The name of the audio file to create</param>
        /// <param name="pStereo">Whether or not the audio file should be stereo (if this is false, the audio file will be mono).</param>
        /// <param name="pSampleRate">The sample rate of the audio file (in Hz)</param>
        /// <param name="pBitsPerSample">The number of bits per sample (8 or 16)</param>
        /// <param name="pOverwrite">Whether or not to overwrite the file if it exists.  If this is false, then a System.IO.IOException will be thrown if the file exists.</param>
        public void Create(string pFilename, bool pStereo, int pSampleRate, short pBitsPerSample, bool pOverwrite)
        {
            // In case a file is currently open, make sure it
            // is closed.  Note: Close() calls InitMembers().
            Close();

            // If the sample rate is not supported, then throw an exception.
            if (!SupportedSampleRate(pSampleRate))
                throw new WAVFileSampleRateException("Unsupported sample rate: " + pSampleRate.ToString(), "WAVFile.Create()", pSampleRate);
            // If the bits per sample is not supported, then throw an exception.
            if (!SupportedBitsPerSample(pBitsPerSample))
                throw new WAVFileBitsPerSampleException("Unsupported number of bits per sample: " + pBitsPerSample.ToString(), "WAVFile.Create()", pBitsPerSample);

            try
            {
                // Create the file.  If pOverwrite is true, then use FileMode.Create to overwrite the
                // file if it exists.  Otherwise, use FileMode.CreateNew, which will throw a
                // System.IO.IOException if the file exists.
                if (pOverwrite)
                    mFileStream = File.Open(pFilename, FileMode.Create);
                else
                    mFileStream = File.Open(pFilename, FileMode.CreateNew);

                mFileMode = WAVFileMode.WRITE;

                // Set the member data from the parameters.
                mNumChannels = pStereo ? (byte)2 : (byte)1;
                mSampleRateHz = pSampleRate;
                mBitsPerSample = pBitsPerSample;

                // Write the parameters to the file header.

                // RIFF chunk (12 bytes total)
                // Write the chunk IDD ("RIFF", 4 bytes)
                byte[] buffer = StrToByteArray("RIFF");
                mFileStream.Write(buffer, 0, 4);
                if (mWAVHeader == null)
                    mWAVHeader = new char[4];
                "RIFF".CopyTo(0, mWAVHeader, 0, 4);
                // File size size (4 bytes) - This will be 0 for now
                Array.Clear(buffer, 0, buffer.GetLength(0));
                mFileStream.Write(buffer, 0, 4);
                // RIFF type ("WAVE")
                buffer = StrToByteArray("WAVE");
                mFileStream.Write(buffer, 0, 4);
                if (mRIFFType == null)
                    mRIFFType = new char[4];
                "WAVE".CopyTo(0, mRIFFType, 0, 4);

                // Format chunk (24 bytes total)
                // "fmt " (ASCII characters)
                buffer = StrToByteArray("fmt ");
                mFileStream.Write(buffer, 0, 4);
                // Length of format chunk (always 16, 4 bytes)
                Array.Clear(buffer, 0, buffer.GetLength(0));
                buffer[0] = 16;
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer);
                mFileStream.Write(buffer, 0, 4);
                // 2 bytes (always 1)
                Array.Clear(buffer, 0, buffer.GetLength(0));
                buffer[0] = 1;
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer, 0, 2);
                mFileStream.Write(buffer, 0, 2);
                // # of channels (2 bytes)
                Array.Clear(buffer, 0, buffer.GetLength(0));
                buffer[0] = mNumChannels;
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer, 0, 2);
                mFileStream.Write(buffer, 0, 2);
                // Sample rate (4 bytes)
                buffer = BitConverter.GetBytes(mSampleRateHz);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer);
                mFileStream.Write(buffer, 0, 4);
                // Calculate the # of bytes per sample: 1=8 bit Mono, 2=8 bit Stereo or
                // 16 bit Mono, 4=16 bit Stereo
                short bytesPerSample = 0;
                if (pStereo)
                    bytesPerSample = (short)((mBitsPerSample / 8) * 2);
                else
                    bytesPerSample = (short)(mBitsPerSample / 8);
                // Write the # of bytes per second (4 bytes)
                mBytesPerSec = mSampleRateHz * bytesPerSample;
                buffer = BitConverter.GetBytes(mBytesPerSec);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer);
                mFileStream.Write(buffer, 0, 4);
                // Write the # of bytes per sample (2 bytes)
                byte[] buffer_2bytes = BitConverter.GetBytes(bytesPerSample);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer_2bytes);
                mFileStream.Write(buffer_2bytes, 0, 2);
                // Bits per sample (2 bytes)
                buffer_2bytes = BitConverter.GetBytes(mBitsPerSample);
                if (!BitConverter.IsLittleEndian)
                    Array.Reverse(buffer_2bytes);
                mFileStream.Write(buffer_2bytes, 0, 2);

                // Data chunk
                // "data" (ASCII characters)
                buffer = StrToByteArray("data");
                mFileStream.Write(buffer, 0, 4);
                // Length of data to follow (4 bytes) - This will be 0 for now
                Array.Clear(buffer, 0, buffer.GetLength(0));
                mFileStream.Write(buffer, 0, 4);
                mDataSizeBytes = 0;

                // Total of 44 bytes written up to this point.

                // The rest of the file is the audio data
            }
            catch (Exception exc)
            {
                throw new WAVFileException(exc.Message, "WAVFile.Create()");
            }
        }

        /// <summary>
        /// Creates a new WAV audio file.  This is an overload that always overwrites the file if it exists.
        /// </summary>
        /// <param name="pFilename">The name of the audio file to create</param>
        /// <param name="pStereo">Whether or not the audio file should be stereo (if this is false, the audio file will be mono).</param>
        /// <param name="pSampleRate">The sample rate of the audio file (in Hz)</param>
        /// <param name="pBitsPerSample">The number of bits per sample (8 or 16)</param>
        public void Create(string pFilename, bool pStereo, int pSampleRate, short pBitsPerSample)
        {
            Create(pFilename, pStereo, pSampleRate, pBitsPerSample, true);
        }

        /// <summary>
        /// Returns whether or not the WAV file format (mono/stereo,
        /// sample rate, and bits per sampe) match another WAV file's
        /// format.
        /// </summary>
        /// <param name="pWAVFile">Another WAVFile object to compare with</param>
        /// <returns></returns>
        public bool FormatMatches(WAVFile pWAVFile)
        {
            bool retval = false;

            if (pWAVFile != null)
                retval = ((mNumChannels == pWAVFile.mNumChannels) &&
                          (mSampleRateHz == pWAVFile.mSampleRateHz) &&
                          (mBitsPerSample == pWAVFile.mBitsPerSample));

            return retval;
        }

        /// <summary>
        /// Returns a WAVFormat struct containing audio format information
        /// (# channels, sample rate, and bits per sample) for a WAV file.
        /// </summary>
        /// <param name="pFilename">The name of the file about which to retrieve format information</param>
        /// <returns>A WAVFormat struct object containing the audio format information for the open file</returns>
        public static WAVFormat GetAudioFormat(string pFilename)
        {
            WAVFormat format = new WAVFormat();

            WAVFile audioFile = new WAVFile();
            if (audioFile.Open(pFilename, WAVFileMode.READ) == "")
            {
                format.BitsPerSample = audioFile.mBitsPerSample;
                format.NumChannels = audioFile.mNumChannels;
                format.SampleRateHz = audioFile.mSampleRateHz;

                audioFile.Close();
            }

            return (format);
        }

        /// <summary>
        /// Returns whether or not a file is a WAV audio file.
        /// </summary>
        /// <param name="pFilename">The name of the file to check</param>
        /// <returns>true if the file is a wave audio file, or false if not</returns>
        static public bool IsWaveFile(string pFilename)
        {
            bool retval = false;

            if (File.Exists(pFilename))
            {
                try
                {
                    FileStream fileStream = File.Open(pFilename, FileMode.Open);
                    // For a WAV file, the first 4 bytes should be "RIFF", and
                    // the RIFF type (3rd set of 4 bytes) should be "WAVE".
                    char[] fileID = new char[4];
                    char[] RIFFType = new char[4];

                    byte[] buffer = new byte[4];

                    // Read the file ID (first 4 bytes)
                    fileStream.Read(buffer, 0, 4);
                    buffer.CopyTo(fileID, 0);

                    // Read the next 4 bytes (but we don't care about this)
                    fileStream.Read(buffer, 0, 4);

                    // Read the RIFF ID (4 bytes)
                    fileStream.Read(buffer, 0, 4);
                    buffer.CopyTo(RIFFType, 0);

                    fileStream.Close();

                    string fileIDStr = new string(fileID);
                    string RIFFTypeStr = new string(RIFFType);

                    retval = ((fileIDStr == "RIFF") && (RIFFTypeStr == "WAVE"));
                }
                catch (Exception exc)
                {
                    throw new WAVFileException(exc.Message, "WAVFile.IsWaveFile()");
                }
            }

            return retval;
        }

        /// <summary>
        /// Returns the highest sample value in a WAV audio file.
        /// The return value is a byte array and will contain one
        /// byte if the file contains 8-bit audio or 2 bytes if the file
        /// contains  16-bit audio.  The return value will be null if
        /// the file cannot be opened.  If it is known that the audio
        /// file contains 16-bit samples, the byte array can be converted
        /// to a 16-bit integer using BitConverter.ToInt16().
        /// </summary>
        /// <param name="pFilename">The name of the WAV file</param>
        /// <param name="pBitsPerSample">This will contain the number of bits per sample, or 0 if the file wasn't loaded.</param>
        /// <returns>A byte array containing the highest audio sample, or null if the file wasn't loaded.</returns>
        public static byte[] HighestSampleValue(string pFilename, out short pBitsPerSample)
        {
            pBitsPerSample = 0;
            byte[] highestSampleValue = null;

            WAVFile audioFile = new WAVFile();
            try
            {
                if (audioFile.Open(pFilename, WAVFileMode.READ) == "")
                {
                    pBitsPerSample = audioFile.mBitsPerSample;

                    if (audioFile.mBitsPerSample == 8)
                    {
                        byte sample = 0;
                        byte highestSample = 0;
                        for (int i = 0; i < audioFile.NumSamples; ++i)
                        {
                            sample = audioFile.GetNextSample_8bit();
                            if (sample > highestSample)
                                highestSample = sample;
                        }

                        highestSampleValue = new byte[1];
                        highestSampleValue[0] = highestSample;
                    }
                    else if (audioFile.mBitsPerSample == 16)
                    {
                        short sample = 0;
                        short highestSample = 0;
                        for (int i = 0; i < audioFile.NumSamples; ++i)
                        {
                            sample = audioFile.GetNextSample_16bit();
                            if (sample > highestSample)
                                highestSample = sample;
                        }

                        highestSampleValue = BitConverter.GetBytes(highestSample);
                        if (!BitConverter.IsLittleEndian)
                            Array.Reverse(highestSampleValue);
                    }

                    audioFile.Close();
                }
            }
            catch (Exception)
            {
            }

            return (highestSampleValue);
        }

        /// <summary>
        /// Returns the highest sample value in a WAV audio file.  This is
        /// a convenience method that can be used when it is known that the
        /// audio file contains 8-bit audio.  If the audio file does not
        /// contain 8-bit audio, this will throw a WAVFileException.
        /// </summary>
        /// <param name="pFilename">The name of the WAV file</param>
        /// <returns>The highest audio sample from the file, or 0 if the file wasn't loaded.</returns>
        public static byte HighestSampleValue_8bit(string pFilename)
        {
            byte highestSampleVal = 0;

            short bitsPerSample;
            byte[] highestSample = HighestSampleValue(pFilename, out bitsPerSample);
            if (highestSample != null)
            {
                if (bitsPerSample == 8)
                    highestSampleVal = highestSample[0];
                else
                    throw new WAVFileReadException("Attempt to get largest 8-bit sample from audio file that is not 8-bit.", "WAVFile.HighestSampleValue_8bit()");
            }

            return highestSampleVal;
        }

        /// <summary>
        /// Returns the highest sample value from multiple audio files, when it is known that
        /// they contain 8-bit audio.
        /// </summary>
        /// <param name="pFilenames">An array containing the names of the files to check</param>
        /// <returns>The highest sample value of all the WAV files</returns>
        public static byte HighestSampleValue_8bit(string[] pFilenames)
        {
            byte sampleValue = 0;
            byte highestSampleValue = 0;

            foreach (string filename in pFilenames)
            {
                sampleValue = HighestSampleValue_8bit(filename);
                if (sampleValue > highestSampleValue)
                    highestSampleValue = sampleValue;
            }

            return highestSampleValue;
        }

        /// <summary>
        /// Returns the highest sample value in a WAV file, as a 16-bit value, regardless of
        /// whether the file contains 8-bit or 16-bit audio.  If the sample is coming from
        /// an 8-bit audio file, the sample will be scaled up from 8-bit to 16-bit.
        /// </summary>
        /// <param name="pFilename">The audio file name</param>
        /// <returns>The highest sample value from the file, as a 16-bit value</returns>
        public static short HighestSampleValueAs16Bit(string pFilename)
        {
            short highestSample = 0;

            try
            {
                WAVFile audioFile = new WAVFile();
                if (audioFile.Open(pFilename, WAVFileMode.READ) == "")
                {
                    if (audioFile.BitsPerSample == 8)
                    {
                        short sample = 0;
                        for (int i = 0; i < audioFile.NumSamples; ++i)
                        {
                            sample = ScaleByteToShort(audioFile.GetNextSample_8bit());
                            if (sample > highestSample)
                                highestSample = sample;
                        }
                    }
                    else if (audioFile.BitsPerSample == 16)
                    {
                        short sample = 0;
                        for (int i = 0; i < audioFile.NumSamples; ++i)
                        {
                            sample = audioFile.GetNextSample_16bit();
                            if (sample > highestSample)
                                highestSample = sample;
                        }
                    }

                    audioFile.Close();
                }
            }
            catch (Exception)
            {
            }

            return highestSample;
        }

        /// <summary>
        /// Returns the highest sample value in a set of WAV files, as a 16-bit value, regardless of
        /// whether the file contains 8-bit or 16-bit audio.
        /// </summary>
        /// <param name="pFilenames">An array containing the filenames</param>
        /// <returns>The highest sample value from the WAV files</returns>
        public static short HighestSampleValueAs16Bit(string[] pFilenames)
        {
            short highestSampleOverall = 0;

            short highestSample = 0;
            foreach (string filename in pFilenames)
            {
                highestSample = HighestSampleValueAs16Bit(filename);
                if (highestSample > highestSampleOverall)
                    highestSampleOverall = highestSample;
            }

            return highestSampleOverall;
        }

        /// <summary>
        /// Returns the highest number of bits per sample in a set of audio files.
        /// </summary>
        /// <param name="pFilenames">An array containing the audio file names</param>
        /// <returns>The highest number of bits per sample in the set of audio files</returns>
        public static short HighestBitsPerSample(string[] pFilenames)
        {
            short bitsPerSample = 0;

            if (pFilenames != null)
            {
                WAVFile audioFile = new WAVFile();
                string retval = "";
                foreach (string filename in pFilenames)
                {
                    try
                    {
                        retval = audioFile.Open(filename, WAVFileMode.READ);
                        if (retval == "")
                        {
                            if (audioFile.BitsPerSample > bitsPerSample)
                                bitsPerSample = audioFile.BitsPerSample;
                            audioFile.Close();
                        }
                    }
                    catch (Exception)
                    {
                    }
                }
            }

            return bitsPerSample;
        }

        /// <summary>
        /// Returns the highest number of channels in a set of audio files.
        /// </summary>
        /// <param name="pFilenames">An array containing the audio file names</param>
        /// <returns>The highest number of channels in the set of audio files</returns>
        public static byte HighestNumChannels(string[] pFilenames)
        {
            byte numChannels = 0;

            if (pFilenames != null)
            {
                WAVFile audioFile = new WAVFile();
                string retval = "";
                foreach (string filename in pFilenames)
                {
                    try
                    {
                        retval = audioFile.Open(filename, WAVFileMode.READ);
                        if (retval == "")
                        {
                            if (audioFile.NumChannels > numChannels)
                                numChannels = audioFile.NumChannels;
                            audioFile.Close();
                        }
                    }
                    catch (Exception)
                    {
                    }
                }
            }

            return numChannels;
        }

        /// <summary>
        /// Adjusts the volume level of a WAV file, saving the adjusted file as a separate file.
        /// </summary>
        /// <param name="pSrcFilename">The name of the WAV file to adjust</param>
        /// <param name="pDestFilename">The name to use for the volume-adjusted WAV file</param>
        /// <param name="pMultiplier">The value by which to multiply the audio samples</param>
        public static void AdjustVolume_Copy(string pSrcFilename, string pDestFilename, double pMultiplier)
        {
            // If an empty source or destination file were passed in, then throw an exception.
            if (pSrcFilename == "")
                throw new WAVFileReadException("Blank filename specified.", "WAVFile.AdjustVolume_Copy()");
            if (pDestFilename == "")
                throw new WAVFileWriteException("Blank filename specified.", "WAVFile.AdjustVolume_Copy()");

            // Open the srouce file
            WAVFile srcFile = new WAVFile();
            string retval = srcFile.Open(pSrcFilename, WAVFileMode.READ);
            if (retval == "")
            {
                // Check to make sure the input file has a supported number of bits/sample and sample rate.  If
                // not, then throw an exception.
                if (!SupportedBitsPerSample(srcFile.BitsPerSample))
                {
                    WAVFileBitsPerSampleException exc = new WAVFileBitsPerSampleException(pSrcFilename +
                                                              " has unsupported bits/sample ("
                                                              + srcFile.BitsPerSample.ToString() + ")",
                                                              "WAVFile.AdjustVolume_Copy()", srcFile.BitsPerSample);
                    srcFile.Close();
                    throw exc;
                }

                // Open the destination file and start copying the adjusted audio data to it.
                WAVFile destFile = new WAVFile();
                destFile.Create(pDestFilename, srcFile.IsStereo, srcFile.SampleRateHz, srcFile.BitsPerSample);
                if (srcFile.BitsPerSample == 8)
                {
                    byte sample = 0;
                    for (int i = 0; i < srcFile.NumSamples; ++i)
                    {
                        // Note: Only multiply the sample if pMultiplier is not 1.0 (if the multiplier is
                        // 1.0, then it would be good to avoid any binary roundoff error).
                        sample = srcFile.GetNextSample_8bit();
                        if (pMultiplier != 1.0)
                            sample = (byte)((double)sample * pMultiplier);
                        destFile.AddSample_8bit(sample);
                    }
                }
                else if (srcFile.BitsPerSample == 16)
                {
                    short sample = 0;
                    for (int i = 0; i < srcFile.NumSamples; ++i)
                    {
                        // Note: Only multiply the sample if pMultiplier is not 1.0 (if the multiplier is
                        // 1.0, then it would be good to avoid any binary roundoff error).
                        sample = srcFile.GetNextSample_16bit();
                        if (pMultiplier != 1.0)
                            sample = (short)((double)sample * pMultiplier);
                        destFile.AddSample_16bit(sample);
                    }
                }

                srcFile.Close();
                destFile.Close();
            }
            else
                throw new WAVFileReadException(retval, "WAVFile.AdjustVolume_Copy()");
        }

        /// <summary>
        /// Changes the volume of a WAV file.
        /// </summary>
        /// <param name="pFilename">The name of the WAV file to adjust</param>
        /// <param name="pMultiplier">The volume multiplier</param>
        public static void AdjustVolumeInPlace(string pFilename, double pMultiplier)
        {
            // If pMultiplier is 1, then we don't need to do anything.
            if (pMultiplier == 1.0)
                return;

            // Open the file
            WAVFile audioFile = new WAVFile();
            string retval = audioFile.Open(pFilename, WAVFileMode.READ_WRITE);
            if (retval == "")
            {
                // Check to make sure the input file has a supported number of bits/sample and sample rate.  If
                // not, then throw an exception.
                if (!SupportedBitsPerSample(audioFile.BitsPerSample))
                {
                    short bitsPerSample = audioFile.BitsPerSample;
                    audioFile.Close();
                    throw new WAVFileBitsPerSampleException(pFilename + " has unsupported bits/sample ("
                                                            + bitsPerSample.ToString() + ")",
                                                            "WAVFile.AdjustVolumeInPlace()", bitsPerSample);
                }
                if (!SupportedSampleRate(audioFile.SampleRateHz))
                {
                    int sampleRate = audioFile.SampleRateHz;
                    audioFile.Close();
                    throw new WAVFileSampleRateException(pFilename + " has unsupported sample rate ("
                                                         + sampleRate.ToString() + ")",
                                                         "WAVFile.AdjustVolumeInPlace()", sampleRate);
                }

                // Adjust the file volume
                if (audioFile.BitsPerSample == 8)
                {
                    byte sample = 0;
                    for (int sampleNum = 0; sampleNum < audioFile.NumSamples; ++sampleNum)
                    {
                        sample = (byte)((double)audioFile.GetNextSample_8bit() * pMultiplier);
                        audioFile.SeekToAudioSample(sampleNum);
                        audioFile.AddSample_8bit(sample);
                    }
                }
                else if (audioFile.BitsPerSample == 16)
                {
                    short sample = 0;
                    for (int sampleNum = 0; sampleNum < audioFile.NumSamples; ++sampleNum)
                    {
                        sample = (short)((double)audioFile.GetNextSample_16bit() * pMultiplier);
                        audioFile.SeekToAudioSample(sampleNum);
                        audioFile.AddSample_16bit(sample);
                    }
                }

                audioFile.Close();
            }
            else
                throw new WAVFileReadException(retval, "WAVFile.AdjustVolumeInPlace()");
        }

        /// <summary>
        /// For 8-bit WAV files: Adjusts the volume level and converts it to a 16-bit audio file.
        /// The converted data is saved to a separate file.
        /// </summary>
        /// <param name="pSrcFilename">The name of the WAV file to convert</param>
        /// <param name="pDestFilename">The name to use for the converted WAV file</param>
        /// <param name="pMultiplier">The volume multiplier</param>
        public static void AdjustVolume_Copy_8BitTo16Bit(string pSrcFilename, string pDestFilename, double pMultiplier)
        {
            // If an empty source or destination file were passed in, then throw an exception.
            if (pSrcFilename == "")
                throw new WAVFileReadException("Blank filename specified.", "WAVFile.AdjustVolume_Copy_8BitTo16Bit()");
            if (pDestFilename == "")
                throw new WAVFileWriteException("Blank filename specified.", "WAVFile.AdjustVolume_Copy_8BitTo16Bit()");

            // Open the srouce file
            WAVFile srcFile = new WAVFile();
            string retval = srcFile.Open(pSrcFilename, WAVFileMode.READ);
            if (retval == "")
            {
                // Check to make sure the input file has 8 bits per sample.  If not, then throw an exception.
                if (srcFile.BitsPerSample != 8)
                {
                    WAVFileBitsPerSampleException exc = new WAVFileBitsPerSampleException(pSrcFilename +
                                                              ": 8 bits per sample required, and the file has " +
                                                              srcFile.BitsPerSample.ToString() + " bits per sample.",
                                                              "WAVFile.AdjustVolume_Copy_8BitTo16Bit()",
                                                              srcFile.BitsPerSample);
                    srcFile.Close();
                    throw exc;
                }

                // Open the destination file
                WAVFile destFile = new WAVFile();
                destFile.Create(pDestFilename, srcFile.IsStereo, srcFile.SampleRateHz, 16, true);

                // Copy the data
                short sample_16bit = 0;
                while (srcFile.NumSamplesRemaining > 0)
                {
                    // Scale the sample from 8-bit to 16 bits
                    sample_16bit = ScaleByteToShort(srcFile.GetNextSample_8bit());

                    // Now, apply pMultiplier if it is not 1.0
                    if (pMultiplier != 1.0)
                        sample_16bit = (short)((double)sample_16bit * pMultiplier);

                    // Save the sample to the destination file
                    destFile.AddSample_16bit(sample_16bit);
                }

                srcFile.Close();
                destFile.Close();
            }
            else
                throw new WAVFileReadException(retval, "WAVFile.AdjustVolume_Copy_8BitTo16Bit()");
        }

        /// <summary>
        /// Converts an 8-bit audio file to a separte 16-bit audio file.
        /// </summary>
        /// <param name="pSrcFilename">The name of the file to convert</param>
        /// <param name="pDestFilename">The name to use for the converted audio file</param>
        public static void Convert_8BitTo16Bit_Copy(string pSrcFilename, string pDestFilename)
        {
            AdjustVolume_Copy_8BitTo16Bit(pSrcFilename, pDestFilename, 1.0);
        }

        /// <summary>
        /// Converts a WAV file's bits/sample and number of channels to a separate WAV file.
        /// </summary>
        /// <param name="pSrcFilename">The name of the file to convert</param>
        /// <param name="pDestFilename">The destination file name</param>
        /// <param name="pBitsPerSample">The destination's number of bits/sample</param>
        /// <param name="pStereo">Whether or not the destination should be stereo</param>
        public static void CopyAndConvert(string pSrcFilename, string pDestFilename, short pBitsPerSample, bool pStereo)
        {
            CopyAndConvert(pSrcFilename, pDestFilename, pBitsPerSample, pStereo, 1.0);
        }

        /// <summary>
        /// Converts a WAV file's bits/sample and number of channels to a separate WAV file.
        /// </summary>
        /// <param name="pSrcFilename">The name of the file to convert</param>
        /// <param name="pDestFilename">The destination file name</param>
        /// <param name="pBitsPerSample">The destination's number of bits/sample</param>
        /// <param name="pStereo">Whether or not the destination should be stereo</param>
        /// <param name="pVolumeMultiplier">A multiplier that can be used to adjust the volume of the output audio file</param>
        public static void CopyAndConvert(string pSrcFilename, string pDestFilename, short pBitsPerSample, bool pStereo, double pVolumeMultiplier)
        {
            WAVFile srcFile = new WAVFile();
            string retval = srcFile.Open(pSrcFilename, WAVFileMode.READ);
            if (retval != "")
                throw new WAVFileException(retval, "WAVFile.Convert_Copy()");

            WAVFile destFile = new WAVFile();
            destFile.Create(pDestFilename, pStereo, srcFile.SampleRateHz, pBitsPerSample);
            if ((srcFile.BitsPerSample == 8) && (pBitsPerSample == 8))
            {
                byte sample = 0;
                if (srcFile.IsStereo && !pStereo)
                {
                    // 8-bit to 8-bit, stereo to mono: Average each 2 samples
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = (byte)((short)((short)srcFile.GetNextSample_8bit() + (short)srcFile.GetNextSample_8bit()) / 2);
                        if (pVolumeMultiplier != 1.0)
                            sample = (byte)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_8bit(sample);
                    }
                }
                else if ((srcFile.IsStereo && pStereo) || (!srcFile.IsStereo && !pStereo))
                {
                    // 8-bit to 8-bit, stereo to stereo or mono to mono
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = srcFile.GetNextSample_8bit();
                        if (pVolumeMultiplier != 1.0)
                            sample = (byte)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_8bit(sample);
                    }
                }
                else if (!srcFile.IsStereo && pStereo)
                {
                    // 8-bit to 8-bit, mono to stereo: Write each sample twice
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = srcFile.GetNextSample_8bit();
                        if (pVolumeMultiplier != 1.0)
                            sample = (byte)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_8bit(sample);
                        destFile.AddSample_8bit(sample);
                    }
                }
            }
            else if ((srcFile.BitsPerSample == 8) && (pBitsPerSample == 16))
            {
                short sample = 0;
                if (srcFile.IsStereo && !pStereo)
                {
                    // 8-bit to 16 bit, stereo to mono: Average each 2 samples
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = (short)((int)((int)srcFile.GetNextSampleAs16Bit() + (int)srcFile.GetNextSampleAs16Bit()) / 2);
                        if (pVolumeMultiplier != 1.0)
                            sample = (short)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_16bit(sample);
                    }
                }
                else if ((srcFile.IsStereo && pStereo) || (!srcFile.IsStereo && !pStereo))
                {
                    // 8-bit to 16 bit, stereo to stereo or mono to mono
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = srcFile.GetNextSampleAs16Bit();
                        if (pVolumeMultiplier != 1.0)
                            sample = (short)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_16bit(sample);
                    }
                }
                else if (!srcFile.IsStereo && pStereo)
                {
                    // 8-bit to 16 bit, mono to stereo: Write each sample twice
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = srcFile.GetNextSampleAs16Bit();
                        if (pVolumeMultiplier != 1.0)
                            sample = (short)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_16bit(sample);
                        destFile.AddSample_16bit(sample);
                    }
                }
            }
            else if ((srcFile.BitsPerSample == 16) && (pBitsPerSample == 8))
            {
                byte sample = 0;
                if (srcFile.IsStereo && !pStereo)
                {
                    // 16-bit to 8-bit, stereo to mono: Average each 2 samples
                    short sample_16bit = 0;
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample_16bit = (short)((int)srcFile.GetNextSample_16bit() + (int)srcFile.GetNextSample_16bit() / 2);
                        if (pVolumeMultiplier != 1.0)
                            sample_16bit = (short)((double)sample_16bit * pVolumeMultiplier);
                        sample = ScaleShortToByte(sample_16bit);
                        destFile.AddSample_8bit(sample);
                    }
                }
                else if ((srcFile.IsStereo && pStereo) || (!srcFile.IsStereo && !pStereo))
                {
                    // 16-bit to 8-bit, stereo to stereo or mono to mono
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = ScaleShortToByte(srcFile.GetNextSample_16bit());
                        if (pVolumeMultiplier != 1.0)
                            sample = (byte)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_8bit(sample);
                    }
                }
                else if (!srcFile.IsStereo && pStereo)
                {
                    // 16-bit to 8-bit, mono to stereo: Write each sample twice
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = ScaleShortToByte(srcFile.GetNextSample_16bit());
                        if (pVolumeMultiplier != 1.0)
                            sample = (byte)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_8bit(sample);
                        destFile.AddSample_8bit(sample);
                    }
                }
            }
            else if ((srcFile.BitsPerSample == 16) && (pBitsPerSample == 16))
            {
                short sample = 0;
                if (srcFile.IsStereo && !pStereo)
                {
                    // 16-bit to 16-bit, stereo to mono: Average each 2 samples
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = (short)((int)((int)srcFile.GetNextSample_16bit() + (int)srcFile.GetNextSample_16bit()) / 2);
                        if (pVolumeMultiplier != 1.0)
                            sample = (short)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_16bit(sample);
                    }
                }
                else if ((srcFile.IsStereo && pStereo) || (!srcFile.IsStereo && !pStereo))
                {
                    // 16-bit to 16-bit, stereo to stereo or mono to mono
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = srcFile.GetNextSample_16bit();
                        if (pVolumeMultiplier != 1.0)
                            sample = (short)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_16bit(sample);
                    }
                }
                else if (!srcFile.IsStereo && pStereo)
                {
                    // 16-bit to 16-bit, mono to stereo: Write each sample twice
                    while (srcFile.NumSamplesRemaining > 0)
                    {
                        sample = srcFile.GetNextSample_16bit();
                        if (pVolumeMultiplier != 1.0)
                            sample = (short)((double)sample * pVolumeMultiplier);
                        destFile.AddSample_16bit(sample);
                        destFile.AddSample_16bit(sample);
                    }
                }
            }

            destFile.Close();
            srcFile.Close();
        }

        /// <summary>
        /// Returns whether or not a sample rate is supported by this class.
        /// </summary>
        /// <param name="pSampleRateHz">A sample rate (in Hz)</param>
        /// <returns>true if the sample rate is supported, or false if not.</returns>
        public static bool SupportedSampleRate(int pSampleRateHz)
        {
            return ((pSampleRateHz == 8000) || (pSampleRateHz == 11025) ||
                    (pSampleRateHz == 16000) || (pSampleRateHz == 18900) ||
                    (pSampleRateHz == 22050) || (pSampleRateHz == 32000) ||
                    (pSampleRateHz == 37800) || (pSampleRateHz == 44056) ||
                    (pSampleRateHz == 44100) || (pSampleRateHz == 48000));
        }

        /// <summary>
        /// Returns whether or not a number of bits per sample is supported by this class.
        /// </summary>
        /// <param name="pBitsPerSample">A number of bits per sample</param>
        /// <returns>true if the bits/sample is supported by this class, or false if not.</returns>
        public static bool SupportedBitsPerSample(short pBitsPerSample)
        {
            return ((pBitsPerSample == 8) || (pBitsPerSample == 16));
        }

        /// <summary>
        /// Merges a set of WAV file sinto a single WAV file in such a way that the audio will be overlayed.
        /// This method will throw a WAVFileException upon error.
        /// </summary>
        /// <param name="pFileList">An array containing the audio filenames</param>
        /// <param name="pOutputFilename">The name of the file to contain the resulting audio</param>
        /// <param name="pTempDir">The full path to the temporary directory to use for the work.  If this directory does not exist, it will be created and then deleted when this method no longer needs it.</param>
        public static void MergeAudioFiles(string[] pFileList, string pOutputFilename, string pTempDir)
        {
            // If pFileList is null or empty, then just return.
            if (pFileList == null)
                return;
            if (pFileList.GetLength(0) == 0)
                return;

            // Make sure all the audio files have the sample rate (we can merge 8-bit and 16-bit audio and
            // convert mono to stereo).  If the sample rates don't match, then throw an exception.
            if (!SampleRatesEqual(pFileList))
                throw new WAVFileAudioMergeException("The sample rates of the audio files differ.", "WAVFile.MergeAudioFiles()");

            // Check the audio format.  If the number of bits/sample or sample rate is not
            // supported, then throw an exception.
            WAVFormat firstFileAudioFormat = GetAudioFormat(pFileList[0]);
            if (!SupportedBitsPerSample(firstFileAudioFormat.BitsPerSample))
                throw new WAVFileBitsPerSampleException("Unsupported number of bits per sample: " + firstFileAudioFormat.BitsPerSample.ToString(), "WAVFile.MergeAudioFiles()", firstFileAudioFormat.BitsPerSample);
            if (!SupportedSampleRate(firstFileAudioFormat.SampleRateHz))
                throw new WAVFileSampleRateException("Unsupported sample rate: " + firstFileAudioFormat.SampleRateHz.ToString(), "WAVFile.MergeAudioFiles()", firstFileAudioFormat.SampleRateHz);

            // 2. Create the temporary directory if it doesn't exist already.  This checks for the
            // existence of the temp directory and stores the result in tempDirExisted so that
            // later, if the temp directory did not exist, we can delete it.
            bool tempDirExisted = Directory.Exists(pTempDir);
            if (!tempDirExisted)
            {
                try
                {
                    Directory.CreateDirectory(pTempDir);
                    if (!Directory.Exists(pTempDir))
                        throw new WAVFileAudioMergeException("Unable to create temporary work directory (" + pTempDir + ")", "WAVFile.MergeAudioFiles()");
                }
                catch (System.Exception exc)
                {
                    throw new WAVFileAudioMergeException("Unable to create temporary work directory (" + pTempDir + "): "
                                                         + exc.Message, "WAVFile.MergeAudioFiles()");
                }
            }

            // 4. Find the highest sample value of all files, and calculate the sound
            //    multiplier based on this (all files will be scaled down by this amount).
            int numTracks = pFileList.GetLength(0);
            double multiplier = 0.0; // The multiplier for scaling down the audio files
            short highestSampleValue = 0; // Will store the highest sample value (8-bit will be cast to short)
            // Determine the highest # of bits per sample in all the audio files.
            short highestBitsPerSample = HighestBitsPerSample(pFileList);
            bool outputStereo = (HighestNumChannels(pFileList) > 1);
            if (highestBitsPerSample == 8)
            {
                // Get the highest sample value of all of the WAV files
                byte highestSample = HighestSampleValue_8bit(pFileList);
                highestSampleValue = (short)highestSample;

                byte difference = (byte)(highestSample - (byte.MaxValue / (byte)numTracks));
                multiplier = 1.0 - ((double)difference / (double)highestSample);
            }
            else if (highestBitsPerSample == 16)
            {
                // Get the highest sample value of all of the WAV files
                highestSampleValue = HighestSampleValueAs16Bit(pFileList);

                short difference = (short)(highestSampleValue - (short.MaxValue / (short)numTracks));
                multiplier = 1.0 - ((double)difference / (double)highestSampleValue);
            }
            if (double.IsInfinity(multiplier) || (multiplier == 0.0))
            {
                // If the temp dir did not exist, then remove it.
                if (!tempDirExisted)
                    DeleteDir(pTempDir);
                // Throw the exception
                throw new WAVFileAudioMergeException("Could not calculate first volume multiplier.", "WAVFile.MergeAudioFiles()");
            }

            if (multiplier < 0.0)
                multiplier = -multiplier;

            // 5. Scale down the audio levels of the source files, and save the output
            //    in the temp directory.  Also change the path to the audio files in
            //    inputFilenames so that they point to the temp directory (we'll be
            //    combining the scaled audio files).
            // This array (scaledAudioFiles) will contain WAVFile objects for the scaled audio files.
            WAVFile[] scaledAudioFiles = new WAVFile[pFileList.GetLength(0)];
            string filename = "";               // For the scaled-down WAV filename
            WAVFile inputFile = new WAVFile();
            WAVFile outputFile = new WAVFile();
            for (int i = 0; i < pFileList.GetLength(0); ++i)
            {
                // pFileList[i] contains the fully-pathed filename.  Using just the
                // filename, construct the fully-pathed filename for the scaled-down
                // version of the file in the temporary directory.
                filename = pTempDir + "\\" + Path.GetFileName(pFileList[i]);

                // Copy the file to the temp directory, adjusting its bits/sample and number of
                // channels if necessary.  And also ajust its volume using multiplier.
                CopyAndConvert(pFileList[i], filename, highestBitsPerSample, outputStereo, multiplier);

                // Create the WAVFile object in the scaledAudioFiles array, and open the scaled
                // audio file with it.
                scaledAudioFiles[i] = new WAVFile();
                scaledAudioFiles[i].Open(filename, WAVFileMode.READ);
            }

            // 7. Now, create the final audio mix file.
            outputFile.Create(pOutputFilename, outputStereo, firstFileAudioFormat.SampleRateHz,
                              highestBitsPerSample);

            // 8. Do the merging..
            // The basic algorithm for doing the merging is as follows:
            // while there is at least 1 sample remaining in any of the source files
            //    sample = 0
            //    for each source file
            //       if the source file has any samples remaining
            //          sample = sample + next available sample from the source file
            //    sample = sample / # of source files
            //    write the sample to the output file
            if (highestBitsPerSample == 8)
            {
                byte sample = 0;
                while (SamplesRemain(scaledAudioFiles))
                {
                    sample = 0;
                    for (int i = 0; i < scaledAudioFiles.GetLength(0); ++i)
                    {
                        if (scaledAudioFiles[i].NumSamplesRemaining > 0)
                            sample += scaledAudioFiles[i].GetNextSample_8bit();
                    }
                    sample /= (byte)(scaledAudioFiles.GetLength(0));
                    outputFile.AddSample_8bit(sample);
                }
            }
            else if (highestBitsPerSample == 16)
            {
                short sample = 0;
                while (SamplesRemain(scaledAudioFiles))
                {
                    sample = 0;
                    for (int i = 0; i < scaledAudioFiles.GetLength(0); ++i)
                    {
                        if (scaledAudioFiles[i].NumSamplesRemaining > 0)
                            sample += scaledAudioFiles[i].GetNextSampleAs16Bit();
                    }
                    sample /= (short)(scaledAudioFiles.GetLength(0));
                    outputFile.AddSample_16bit(sample);
                }
            }
            outputFile.Close();

            // 9. Remove the input files(to free up disk space.
            foreach (WAVFile audioFile in scaledAudioFiles)
            {
                filename = audioFile.Filename;
                audioFile.Close();
                File.Delete(filename);
            }

            // 10. Now, increase the volume level of the output file. (will first have
            //     to see if the inputs are 8-bit or 16-bit.)
            //  Adjust the volume level of the file so that its volume is
            //  the same as the volume of the input files.  This is done by
            //  first finding the highest sample value of all files, then
            //  the highest sample value of the combined audio file, and
            //  scaling the audio of the output file to match the volume
            //  of the input files (such that the highest level of the output
            //  file is the same as the highest level of all files).
            // For 16-bit audio, this works okay, but for 8-bit audio, the
            //  output seems to sound better if we adjust the volume so that
            //  the highest sample value is 3/4 of the maximum sample value
            //  for the # of bits/sample.
            if (highestBitsPerSample == 8)
            {
                byte highestSampleVal = WAVFile.HighestSampleValue_8bit(pOutputFilename);
                byte maxValue = byte.MaxValue / 4 * 3;
                multiplier = (double)maxValue / (double)highestSampleVal;
            }
            else if (highestBitsPerSample == 16)
            {
                short finalMixFileHighestSample = WAVFile.HighestSampleValueAs16Bit(pOutputFilename);
                // Calculate the multiplier for adjusting the audio of the final mix file.
                //short difference = (short)(finalMixFileHighestSample - highestSampleValue);
                //multiplier = 1.0 - ((double)difference / (double)finalMixFileHighestSample);
                // This calculates the multiplier based on the highest sample value in the audio
                // file and the highest possible 16-bit sample value.
                multiplier = (double)short.MaxValue / (double)finalMixFileHighestSample;
            }
            if (multiplier < 0.0)
                multiplier = -multiplier;

            // Adjust the volume of the final mix file.
            AdjustVolumeInPlace(pOutputFilename, multiplier);

            // If the temporary directory did not exist prior to this method being called, then
            // delete it.
            if (!tempDirExisted)
            {
                string retval = DeleteDir(pTempDir);
                if (retval != "")
                    throw new WAVFileAudioMergeException("Unable to remove temp directory (" + pTempDir + "): " + retval,
                                                         "WAVFile.MergeAudioFiles()");
            }
        }

        /// <summary>
        /// Moves the file pointer back to the start of the audio data.
        /// </summary>
        public void SeekToAudioDataStart()
        {
            SeekToAudioSample(0);
            // Update mSamplesRemaining - but this is only necessary for read or read/write mode.
            if ((mFileMode == WAVFileMode.READ) || (mFileMode == WAVFileMode.READ_WRITE))
                mNumSamplesRemaining = NumSamples;
        }

        /// <summary>
        /// Moves the file pointer to a given audio sample number.
        /// </summary>
        /// <param name="pSampleNum">The sample number to which to move the file pointer</param>
        public void SeekToAudioSample(long pSampleNum)
        {
            if (mFileStream != null)
            {
                // Figure out the byte position.  This will be mDataStartPos + however many
                // bytes per sample * pSampleNum.
                long bytesPerSample = mBitsPerSample / 8;
                try
                {
                    mFileStream.Seek(mDataStartPos + (bytesPerSample * pSampleNum), 0);
                }
                catch (System.IO.IOException exc)
                {
                    throw new WAVFileIOException("Unable to to seek to sample " + pSampleNum.ToString() + ": " + exc.Message,
                                                 "WAVFile.SeekToAudioSample()");
                }
                catch (System.NotSupportedException exc)
                {
                    throw new WAVFileIOException("Unable to to seek to sample " + pSampleNum.ToString() + ": " + exc.Message,
                                                 "WAVFile.SeekToAudioSample()");
                }
                catch (Exception exc)
                {
                    throw new WAVFileException(exc.Message, "WAVFile.SeekToAudioSample()");
                }
            }
        }

        /// <summary>
        /// Returns a string representation of a WAVFileMode enumeration value.
        /// </summary>
        /// <param name="pMode">A value of the WAVFileMode enumeration</param>
        /// <returns>A string representation of pMode</returns>
        public static string WAVFileModeStr(WAVFileMode pMode)
        {
            string retval = "";

            switch (pMode)
            {
                case WAVFileMode.READ:
                    retval = "READ";
                    break;
                case WAVFileMode.WRITE:
                    retval = "WRITE";
                    break;
                case WAVFileMode.READ_WRITE:
                    retval = "READ_WRITE";
                    break;
                default:
                    retval = "Unknown";
                    break;
            }

            return retval;
        }

        ////////////////
        // Properties //
        ////////////////

        /// <summary>
        /// Gets the name of the file that was opened.
        /// </summary>
        public string Filename
        {
            get { return mFilename; }
        }

        /// <summary>
        /// Gets the WAV file header as read from the file (array of 4 chars)
        /// </summary>
        public char[] WAVHeader
        {
            get { return mWAVHeader; }
        }

        /// <summary>
        /// Gets the WAV file header as read from the file, as a String object
        /// </summary>
        public string WAVHeaderString
        {
            get
            {
                return new string(mWAVHeader);
            }
        }

        /// <summary>
        /// Gets the RIFF type as read from the file (array of chars)
        /// </summary>
        public char[] RIFFType
        {
            get { return mRIFFType; }
        }

        /// <summary>
        /// Gets the RIFF type as read from the file, as a String object
        /// </summary>
        public string RIFFTypeString
        {
            get
            {
                return new string(mRIFFType);
            }
        }

        /// <summary>
        /// Gets the audio file's number of channels
        /// </summary>
        public byte NumChannels
        {
            get { return mNumChannels; }
        }

        /// <summary>
        /// Gets whether or not the file is stereo.
        /// </summary>
        public bool IsStereo
        {
            get { return (mNumChannels == 2); }
        }

        /// <summary>
        /// Gets the audio file's sample rate (in Hz)
        /// </summary>
        public int SampleRateHz
        {
            get { return mSampleRateHz; }
        }

        /// <summary>
        /// Gets the number of bytes per second for the audio file
        /// </summary>
        public int BytesPerSec
        {
            get { return mBytesPerSec; }
        }

        /// <summary>
        /// Gets the number of bytes per sample for the audio file
        /// </summary>
        public short BytesPerSample
        {
            get { return mBytesPerSample; }
        }

        /// <summary>
        /// Gets the number of bits per sample for the audio file
        /// </summary>
        public short BitsPerSample
        {
            get { return mBitsPerSample; }
        }

        /// <summary>
        /// Gets the data size (in bytes) for the audio file.  This is read from a field
        /// in the WAV file header.
        /// </summary>
        public int DataSizeBytes
        {
            get { return mDataSizeBytes; }
        }

        /// <summary>
        /// Gets the file size (in bytes).  This is read from a field in the WAV file header.
        /// </summary>
        public long FileSizeBytes
        {
            //get { return mFileSizeBytes; }
            get { return mFileStream.Length; }
        }

        /// <summary>
        /// Gets the number of audio samples in the WAV file.  This is calculated based on
        /// the data size read from the file and the number of bits per sample.
        /// </summary>
        public int NumSamples
        {
            get { return (mDataSizeBytes / (int)(mBitsPerSample / 8)); }
        }

        /// <summary>
        /// Gets the number of samples remaining (when in read mode).
        /// </summary>
        public int NumSamplesRemaining
        {
            get { return mNumSamplesRemaining; }
        }

        /// <summary>
        /// Gets the mode of the open file (read, write, or read/write)
        /// </summary>
        public WAVFileMode FileOpenMode
        {
            get { return mFileMode; }
        }

        // Property that uses 
        /// <summary>
        /// Gets a WAVFormat object containing the audio format information
        /// for the file (# channels, sample rate, and bits per sample).
        /// </summary>
        public WAVFormat AudioFormat
        {
            get
            {
                return(new WAVFormat(mNumChannels, mSampleRateHz, mBitsPerSample));
            }
        }

        /// <summary>
        /// Gets the current file byte position.
        /// </summary>
        public long FilePosition
        {
            get { return (mFileStream != null ? mFileStream.Position : 0); }
        }

        /////////////////////
        // Private members //
        /////////////////////

        /// <summary>
        /// Initializes the data members (for the constructors).
        /// </summary>
        private void InitMembers()
        {
            mFilename = null;
            mFileStream = null;
            mWAVHeader = new char[4];
            mDataSizeBytes = 0;
            mBytesPerSample = 0;
            //mFileSizeBytes = 0;
            mRIFFType = new char[4];

            // These audio format defaults correspond to the standard for
            // CD audio.
            mNumChannels = 2;
            mSampleRateHz = 44100;
            mBytesPerSec = 176400;
            mBitsPerSample = 16;

            mFileMode = WAVFileMode.READ;

            mNumSamplesRemaining = 0;
        }

        /// <summary>
        /// Returns whether or not the sample rates in a set of WAV files are equal.
        /// </summary>
        /// <param name="pFileList">An array containing the names of the files to check</param>
        /// <returns>Whether or not all WAV files have the same sample rate</returns>
        private static bool SampleRatesEqual(string[] pFileList)
        {
            bool sampleRatesMatch = true;

            if (pFileList != null)
            {
                int numFiles = pFileList.GetLength(0);
                if (numFiles > 1)
                {
                    int firstSampleRate = GetAudioFormat(pFileList[0]).SampleRateHz;
                    for (int i = 1; i < numFiles; ++i)
                    {
                        if (GetAudioFormat(pFileList[i]).SampleRateHz != firstSampleRate)
                        {
                            sampleRatesMatch = false;
                            break;
                        }
                    }
                }
            }
            else
                sampleRatesMatch = false;

            return sampleRatesMatch;
        }

        /// <summary>
        /// Returns whether or not any audio samples are remaining to be read from an array of WAV files.
        /// </summary>
        /// <param name="WAVFileArray">An array of WAVFile objects that are currently open</param>
        /// <returns>true if there are samples that can still be read from any of the files, or false if not.</returns>
        static private bool SamplesRemain(WAVFile[] WAVFileArray)
        {
            bool samplesRemain = false;

            if (WAVFileArray != null)
            {
                for (int i = 0; i < WAVFileArray.GetLength(0); ++i)
                {
                    if (WAVFileArray[i].NumSamplesRemaining > 0)
                    {
                        samplesRemain = true;
                        break;
                    }
                }
            }

            return (samplesRemain);
        }

        /// <summary>
        /// Removes a directory, including all the files in it.  This is used by MergeAudioFiles().
        /// </summary>
        /// <param name="pPath">The directory to remove</param>
        /// <returns>A blank string upon success, or a warning upon error.  If the directory does not exist, that is not an error.</returns>
        private static string DeleteDir(string pPath)
        {
            string retval = "";

            if (Directory.Exists(pPath))
            {
                try
                {
                    // Delete all the files
                    string[] filenames = Directory.GetFiles(pPath);
                    foreach (string filename in filenames)
                        File.Delete(filename);
                    // Delete the directory
                    Directory.Delete(pPath, true);
                }
                catch (System.Exception exc)
                {
                    retval = exc.Message;
                }
            }

            return retval;
        }

        /// <summary>
        /// Converts a string to a byte array.  The source for this came
        /// from http://www.chilkatsoft.com/faq/DotNetStrToBytes.html .
        /// </summary>
        /// <param name="pStr">A String object</param>
        /// <returns>A byte array containing the data from the String object</returns>
        private static byte[] StrToByteArray(string pStr)
        {
            System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
            return encoding.GetBytes(pStr);
        }

        /// <summary>
        /// Scales a byte value to a 16-bit (short) value by calculating the value's percentage of
        /// maximum for 8-bit values, then calculating the 16-bit value with that
        /// percentage.
        /// </summary>
        /// <param name="pByteVal">A byte value to convert</param>
        /// <returns>The 16-bit scaled value</returns>
        private static short ScaleByteToShort(byte pByteVal)
        {
            short val_16bit = 0;
            double scaleMultiplier = 0.0;
            if (pByteVal > 0)
            {
                scaleMultiplier = (double)pByteVal / (double)byte.MaxValue;
                val_16bit = (short)((double)short.MaxValue * scaleMultiplier);
            }
            else if (pByteVal < 0)
            {
                scaleMultiplier = (double)pByteVal / (double)byte.MinValue;
                val_16bit = (short)((double)short.MinValue * scaleMultiplier);
            }

            return val_16bit;
        }

        /// <summary>
        /// Scales a 16-bit (short) value to an 8-bit (byte) value by calculating the
        /// value's percentage of maximum for 16-bit values, then calculating the 8-bit
        /// value with that percentage.
        /// </summary>
        /// <param name="pShortVal">A 16-bit value to convert</param>
        /// <returns>The 8-bit scaled value</returns>
        private static byte ScaleShortToByte(short pShortVal)
        {
            byte val_8bit = 0;
            double scaleMultiplier = 0.0;
            if (pShortVal > 0)
            {
                scaleMultiplier = (double)pShortVal / (double)short.MaxValue;
                val_8bit = (byte)((double)byte.MaxValue * scaleMultiplier);
            }
            else if (pShortVal < 0)
            {
                scaleMultiplier = (double)pShortVal / (double)short.MinValue;
                val_8bit = (byte)((double)byte.MinValue * scaleMultiplier);
            }

            return val_8bit;
        }

        private string mFilename;       // The name of the file open
        private FileStream mFileStream; // For reading the audio file
        // File header information
        private char[] mWAVHeader;      // The WAV header (4 bytes, "RIFF")
        //private int mFileSizeBytes;     // The file size, in bytes
        private char[] mRIFFType;       // The RIFF type (4 bytes, "WAVE")
        // Audio format information
        private byte mNumChannels;      // The # of channels (1 or 2)
        private int mSampleRateHz;      // The audio sample rate (Hz)
        private int mBytesPerSec;       // Bytes per second
        private short mBytesPerSample;  // # of bytes per sample (1=8 bit Mono, 2=8 bit Stereo or 16 bit Mono, 4=16 bit Stereo)
        private short mBitsPerSample;   // # of bits per sample
        private int mDataSizeBytes;     // The data size (bytes)
        private WAVFileMode mFileMode;  // Specifies the file mode (read, write)

        //private int mDataBytesWritten;  // Used in write mode for keeping track of
                                        // the number of bytes written
        private int mNumSamplesRemaining; // When in read mode, this is used for keeping track of how many audio
                                          // samples remain.  This is updated in GetNextSample_ByteArray().

        // Static members
        private static short mDataStartPos; // The byte position of the start of the audio data
    }
}
