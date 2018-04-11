/*
 * Copyright 2018 Picovoice Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package ai.picovoice.porcupine;


import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

/**
 * Utility class to read Wav files.
 *
 * This is a minimal implementation for testing purposes. It expects the file to have single channel
 * and has a sample rate of 16000 samples per second.
 */
class WavReader {
    // Identifiers in the header of the file.
    private final static int FMT_CHUNK_ID = 0x20746D66;
    private final static int DATA_CHUNK_ID = 0x61746164;
    private final static int RIFF_CHUNK_ID = 0x46464952;
    private final static int RIFF_TYPE_ID = 0x45564157;
    // Size of the buffer that holds the raw PCM data.
    private final static int BUFFER_SIZE = 4096;

    // information about the data in the WAV file.
    private long bytesPerSample;
    private long numFrames;

    private final byte[] buffer = new byte[BUFFER_SIZE];
    private FileInputStream inputStream;

    private int bufferPointer;
    private int bytesRead;
    private long frameCounter;

    /**
     * Initialize WaveReader.
     * @param file file object to the WAV file.
     * @throws IOException if there are any errors while reading the WAV file.
     */
    WavReader(File file) throws IOException {
        this.inputStream = new FileInputStream(file);
        // Read the header.
        int r = this.inputStream.read(this.buffer, 0, 12);
        if (r != 12) {
            throw new IOException("Not enough data in the header.");
        }
        // Read the sections in the header.
        long riffChunkID = getLEValue(this.buffer, 0, 4);
        long chunkSize = getLEValue(this.buffer, 4, 4);
        long riffTypeID = getLEValue(this.buffer, 8, 4);

        if (riffChunkID != RIFF_CHUNK_ID) {
            throw new IOException("Incorrect riff chunk ID");
        }
        if (riffTypeID != RIFF_TYPE_ID) {
            throw new IOException("Incorrect riff type ID");
        }
        if (file.length() != chunkSize + 8) {
            throw new IOException("File size mismatch");
        }

        long blockAlign = 0;
        boolean foundFormat = false;
        boolean foundData;

        // try to read the format and data chunks.
        while (true) {
            r = this.inputStream.read(this.buffer, 0, 8);
            if (r == -1) {
                throw new IOException("Cannot read format chunk");
            }
            if (r != 8) {
                throw new IOException("Cannot read chunk header");
            }

            long chunkID = getLEValue(this.buffer, 0, 4);
            chunkSize = getLEValue(this.buffer, 4, 4);
            long numChunkBytes = (chunkSize % 2 == 1) ? chunkSize + 1 : chunkSize;

            if (chunkID == FMT_CHUNK_ID) {
                foundFormat = true;
                this.inputStream.read(this.buffer, 0, 16);

                long compressionCode = getLEValue(this.buffer, 0, 2);
                if (compressionCode != 1) {
                    throw new IOException("Compression is not supported");
                }

                long numChannels = getLEValue(this.buffer, 2, 2);
                long sampleRate = getLEValue(this.buffer, 4, 4);
                blockAlign = getLEValue(this.buffer, 12, 2);
                long validBits = getLEValue(this.buffer, 14, 2);

                if (numChannels != 1) {
                    throw new IOException("Can only read wav files with one channel");
                }
                if (blockAlign == 0) {
                    throw new IOException("Block Align cannot be zero");
                }
                if (validBits < 2) {
                    throw new IOException("Valid Bits cannot be less than 2");
                }
                if (validBits > 64) {
                    throw new IOException("Valid Bits is greater than 64");
                }

                if (sampleRate != 16000) {
                    throw new IOException("Sample rate should be 16000");
                }

                this.bytesPerSample = (validBits + 7) / 8;
                if (this.bytesPerSample * numChannels != blockAlign) {
                    throw new IOException("Block Align value does not match");
                }

                numChunkBytes -= 16;
                if (numChunkBytes > 0) {
                    this.inputStream.skip(numChunkBytes);
                }
            } else if (chunkID == DATA_CHUNK_ID) {
                if (!foundFormat) {
                    throw new IOException("Data chunk found before format chunk");
                }
                if (chunkSize % blockAlign != 0) {
                    throw new IOException("Data Chunk size is not multiple of block align");
                }
                this.numFrames = chunkSize / blockAlign;
                foundData = true;
                break;
            } else {
                this.inputStream.skip(numChunkBytes);
            }
        }

        if (!foundData) {
            throw new IOException("Did not find a data chunk");
        }
        this.bufferPointer = 0;
        this.bytesRead = 0;
        this.frameCounter = 0;
    }

    /**
     * Read frames from WAV file.
     * @param buffer The buffer to put PCm data into.
     * @param numFrames Number of frames to read from the WAV file.
     * @return Numbers of frames read from the WAV file.
     * @throws IOException if there is an error while reading from the WAV file.
     */
    int readFrames(short[] buffer, int numFrames) throws IOException {
        for (int i = 0 ; i < numFrames ; ++i) {
            if (frameCounter == this.numFrames) return i;
            buffer[i] = (short) readSample();
            ++frameCounter;
        }
        return numFrames;
    }

    /**
     * Close the input stream.
     * @throws IOException if there is any error while closing the stream.
     */
    void close() throws IOException {
        if (inputStream != null) {
            inputStream.close();
            inputStream = null;
        }
    }

    /**
     * Get the little endian value from the buffer.
     * @param buffer The input buffer to read data from.
     * @param pos The start position in the buffer.
     * @param numBytes Number of bytes to read from buffer.
     * @return the little endian value.
     */
    private long getLEValue(byte[] buffer, int pos, int numBytes) {
        --numBytes;
        pos += numBytes;
        long val = buffer[pos] & 0xFF;
        for (int b = 0 ; b < numBytes ; ++b) {
            val = (val << 8) + (buffer[--pos] & 0xFF);
        }
        return val;
    }

    /**
     * Read a sample value from WAV file.
     * @return The sample value from WAV file.
     * @throws IOException if there is an error while reading the file.
     */
    private long readSample() throws IOException {
        long val = 0;
        for (int b = 0 ; b < bytesPerSample ; ++b) {
            if (bufferPointer == bytesRead)
            {
                int read = inputStream.read(buffer, 0, BUFFER_SIZE);
                if (read == -1) {
                    throw new IOException("Not enough data available");
                }
                bytesRead = read;
                bufferPointer = 0;
            }

            int v = buffer[bufferPointer];
            if (b < bytesPerSample - 1 || bytesPerSample == 1) v &= 0xFF;
            val += v << (b * 8);
            ++bufferPointer;
        }
        return val;
    }
}