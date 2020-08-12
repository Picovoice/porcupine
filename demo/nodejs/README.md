# NodeJS Demos

## File-based demo

### Quick Start

The file-based demo allows you to scan a wave file for keywords using Porcupine. Note: **The demo requires 16KHz, 16-bit, linear PCM, single-channel (mono) WAV files**.

To run the file-based demo,

e.g. check the input wave file for the built-in keyword "bumblebee" against the audio sample that is included with the porcupine repository:

```bash
cd demo/nodejs

yarn demo --input_audio_file_path ../../resources/audio_samples/multiple_keywords.wav --keywords grasshopper
```

To see the full set of options, use `--help`:

```
yarn demo --help
```

### Sensitivity

The sensitivity is a floating point value in the range [0,1] which specifies the tradeoff between miss rate and false alarm. The demo defaults to 0.5. You can override this with `--sensitivity`:

```bash
yarn demo --input_audio_file_path ../../resources/audio_samples/multiple_keywords.wav --keywords bumblebee --sensitivity 0.65
```

### Custom keyword (ppn) files

Porcupine includes several built-in keywords, which are stored as `.ppn` files. To train custom PPN files, see the [Picovoice Console](https://picovoice.ai/console/).

Unlike the built-in keywords, custom PPN files generated with the Picovoice Console carry restrictions including (but not limited to): training allowance, time limits, available platforms, and commercial usage.

#### Providing custom ppn files

You can specify non-builtin keywords as absolute paths to `.ppn` keyword files, using `--keyword_file_paths`. The PPN keyword files must be compatible with the platform you are running.

e.g. for Linux (x86_64), using two keywords: "bumblebee" and "grasshopper":

```bash
yarn demo \
--input_audio_file_path ../../resources/audio_samples/multiple_keywords.wav \
--keyword_file_paths ../../resources/keyword_files/linux/grasshopper_linux.ppn,../../resources/keyword_files/linux/bumblebee_linux.ppn
```

### Custom library and model files

You may override the porcupine model and dynamic libraries by specifying their absolute paths with `--model_file_path` and `--library_file_path`, respectively. As with keyword files, the dynamic library is specific to the platform.

e.g. for macOS (x86_64):

```bash
yarn demo \
--input_audio_file_path ../../resources/audio_samples/multiple_keywords.wav \
--keywords terminator \
--library_file_path ../../lib/mac/x86_64/libpv_porcupine.dylib \
--model_file_path ../../lib/common/porcupine_params.pv
```
