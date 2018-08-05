# Porcupine Optimizer

The optimizer enables developers to build models for their wake words of choice. Below it is assumed that the current 
working directory is the root of the repository. ```${SYSTEM}``` refers to the operating system (linux, mac, or windows) and
```${MACHINE}``` refers to CPU architecture.

# Usage

The optimizer utility is located at [/tools/optimizer](/tools/optimizer).
```tools/optimizer/${SYSTEM}/${MACHINE}/pv_porcupine_optimizer -h``` gives the usage string as below

```
 [INFO] Usage: pv_porcupine_optimizer -r resource_directory -w keyword -p platform -o output_directory
 [INFO] Optimizes keyword spotting model (aka porcupine) for a given keyword and stores the result into a file.
 [INFO] 
 [INFO]  -r Absolute path to resource directory that contains data needed by optimizer.
 [INFO]  -w Keyword.
 [INFO]  -p Target platform for running keyword spotting module. Valid options are: linux, mac, and windows.
 [INFO]  -o Absolute path to output directory where keyword file will be stored.
```

In this repository, the resource directory is located at [/resources](/resources). Keyword files are
platform dependent. This means that a keyword file generated with ```-p linux``` cannot run on Android or Mac. The
optimizer available at this repository can produce keyword files for Linux, Mac, and Windows platforms. Custom keyword files for other
platforms are provided with purchase of the commercial license. In order to purchase a commercial license contact contact@picovoice.ai.

A keyword file for **Vancouver** targeted for a Mac machine can be created using

```bash
tools/optimizer/${SYSTEM}/${MACHINE}/pv_porcupine_optimizer -r resources -w "vancouver" -p mac -o ~/
```

# FAQ

#### How do I create keyword files for platforms other than Linux/Mac/Windows?

The optimizer provided in this repository does not have the capability to do so. Custom keyword files for 
platforms other than Linux, Mac, or Windows are only provided with purchase of the commercial license.

#### How do I deal with "[ERROR] could not find the pronunciation of XXX"?

The optimizer has information about tens of thousands of commonly-used English words. That being said, your chosen keyword might
not be available in its internal dictionary. Please open an issue and we will add the word to optimizer's internal dictionary
for next release.


#### How do I deal with "[ERROR] Wake phrase is too short or long for this optimizer."?

This happens when the keyword is very short (e.g. "No") or very long (e.g. "this is a very descriptive keyword"). The 
optimizer expects a certain length range for wake phrase and throws this error if the length is out of range. A typical 
wake phrase is expected to be either a single word with multiple syllables (e.g. "Alexa") or a two word phrase 
(e.g. "Hey Siri" or "OK Google").
