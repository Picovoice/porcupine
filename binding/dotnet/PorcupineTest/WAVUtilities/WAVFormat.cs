/* This file contains the WAVFormat struct, which contains audio format
 * information and is used by the WAVFile class.
 *
 * Date       Author         Description
 * 2009-03-13 erico          Created
 */

namespace PorcupineCS
{
    // This struct contains audio format information and is used
    // by the WAVFile class.
    public struct WAVFormat
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="pNumChannels">The number of channels</param>
        /// <param name="pSampleRateHz">The sample rate (Hz)</param>
        /// <param name="pBitsPerSample">The number of bits per sample</param>
        public WAVFormat(byte pNumChannels, int pSampleRateHz, short pBitsPerSample)
        {
            mNumChannels = pNumChannels;
            mSampleRateHz = pSampleRateHz;
            mBitsPerSample = pBitsPerSample;
        }

        public static bool operator ==(WAVFormat pWAVFormat1, WAVFormat pWAVFormat2)
        {
            return ((pWAVFormat1.mNumChannels == pWAVFormat2.mNumChannels) &&
                   (pWAVFormat1.mSampleRateHz == pWAVFormat2.mSampleRateHz) &&
                   (pWAVFormat1.mBitsPerSample == pWAVFormat2.mBitsPerSample));
        }

        public static bool operator !=(WAVFormat pWAVFormat1, WAVFormat pWAVFormat2)
        {
            return ((pWAVFormat1.mNumChannels != pWAVFormat2.mNumChannels) ||
                   (pWAVFormat1.mSampleRateHz != pWAVFormat2.mSampleRateHz) ||
                   (pWAVFormat1.mBitsPerSample != pWAVFormat2.mBitsPerSample));
        }

        public override bool Equals(object obj)
        {
            if ((obj == null) || (GetType() != obj.GetType())) return false;
            WAVFormat format = (WAVFormat)obj;
            return (this == format);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        ////////////////
        // Properties //
        ////////////////

        public byte NumChannels
        {
            get { return mNumChannels; }
            set { mNumChannels = value; }
        }

        public bool IsStereo
        {
            get { return (mNumChannels == 2); }
        }

        public int SampleRateHz
        {
            get { return mSampleRateHz; }
            set { mSampleRateHz = value; }
        }

        public short BitsPerSample
        {
            get { return mBitsPerSample; }
            set { mBitsPerSample = value; }
        }

        // Data members
        private byte mNumChannels;      // The # of channels (1 or 2)
        private int mSampleRateHz;      // The audio sample rate (Hz)
        private short mBitsPerSample;   // # of bits per sample
    }
}