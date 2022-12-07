/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use chrono::Duration;
use clap::{App, Arg, ArgGroup};
use itertools::Itertools;
use porcupine::{BuiltinKeywords, PorcupineBuilder};
use std::path::PathBuf;
use std::str::FromStr;

fn porcupine_demo(
    input_audio_path: PathBuf,
    access_key: &str,
    keywords_or_paths: KeywordsOrPaths,
    sensitivities: Option<Vec<f32>>,
    model_path: Option<&str>,
) {
    let mut porcupine_builder = match keywords_or_paths {
        KeywordsOrPaths::Keywords(ref keywords) => {
            PorcupineBuilder::new_with_keywords(access_key, keywords)
        }
        KeywordsOrPaths::KeywordPaths(ref keyword_paths) => {
            PorcupineBuilder::new_with_keyword_paths(access_key, keyword_paths)
        }
    };

    if let Some(sensitivities) = sensitivities {
        porcupine_builder.sensitivities(&sensitivities);
    }

    if let Some(model_path) = model_path {
        porcupine_builder.model_path(model_path);
    }

    let porcupine = porcupine_builder
        .init()
        .expect("Failed to create Porcupine");

    let mut wav_reader = match hound::WavReader::open(input_audio_path.clone()) {
        Ok(reader) => reader,
        Err(err) => panic!(
            "Failed to open .wav audio file {}: {}",
            input_audio_path.display(),
            err
        ),
    };

    if wav_reader.spec().sample_rate != porcupine.sample_rate() {
        panic!(
            "Audio file should have the expected sample rate of {}, got {}",
            porcupine.sample_rate(),
            wav_reader.spec().sample_rate
        );
    }

    if wav_reader.spec().channels != 1u16 {
        panic!(
            "Audio file should have the expected number of channels 1, got {}",
            wav_reader.spec().channels
        );
    }

    if wav_reader.spec().bits_per_sample != 16u16
        || wav_reader.spec().sample_format != hound::SampleFormat::Int
    {
        panic!("WAV format should be in the signed 16 bit format",);
    }

    let mut timestamp = Duration::zero();
    for frame in &wav_reader
        .samples()
        .chunks(porcupine.frame_length() as usize)
    {
        let frame: Vec<i16> = frame.map(|s| s.unwrap()).collect_vec();
        timestamp = timestamp
            + Duration::milliseconds(
                ((1000 * frame.len()) / porcupine.sample_rate() as usize) as i64,
            );

        if frame.len() == porcupine.frame_length() as usize {
            let keyword_index = porcupine.process(&frame).unwrap();
            if keyword_index >= 0 {
                println!(
                    "[{}:{}:{}] Detected {}",
                    timestamp.num_minutes(),
                    timestamp.num_seconds() - (timestamp.num_minutes() * 60),
                    timestamp.num_milliseconds() - (timestamp.num_seconds() * 1000),
                    keywords_or_paths.get(keyword_index as usize)
                );
            }
        }
    }
}

#[derive(Clone)]
enum KeywordsOrPaths {
    Keywords(Vec<BuiltinKeywords>),
    KeywordPaths(Vec<PathBuf>),
}

impl KeywordsOrPaths {
    fn get(&self, index: usize) -> String {
        match self {
            Self::Keywords(keywords) => keywords[index].to_str().to_string(),
            Self::KeywordPaths(keyword_paths) => keyword_paths[index]
                .clone()
                .into_os_string()
                .into_string()
                .unwrap(),
        }
    }
}

fn main() {
    let matches = App::new("Picovoice Porcupine Rust File Demo")
        .arg(
            Arg::with_name("input_audio_path")
            .long("input_audio_path")
            .value_name("PATH")
            .help("Path to input audio file (mono, WAV, 16-bit, 16kHz).")
            .takes_value(true)
            .required(true)
        )
        .arg(
            Arg::with_name("access_key")
                .long("access_key")
                .value_name("ACCESS_KEY")
                .help("AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)")
                .takes_value(true)
                .required(true),
        )
        .group(
            ArgGroup::with_name("keywords_group")
            .arg("keywords")
            .arg("keyword_paths")
            .required(true)
        )
        .arg(
            Arg::with_name("keywords")
            .long("keywords")
            .value_name("KEYWORDS")
            .use_delimiter(true)
            .help("Comma-separated list of default keywords for detection.")
            .takes_value(true)
            .possible_values(&BuiltinKeywords::options())
        )
        .arg(
            Arg::with_name("keyword_paths")
            .long("keyword_paths")
            .value_name("PATHS")
            .use_delimiter(true)
            .help("Comma-separated list of paths to keyword model files. If not set it will be populated from `--keywords` argument.")
            .takes_value(true)
        )
        .arg(
            Arg::with_name("model_path")
            .long("model_path")
            .value_name("PATH")
            .help("Path to the file containing model parameter.")
            .takes_value(true)
        )
        .arg(
            Arg::with_name("sensitivities")
            .long("sensitivities")
            .value_name("SENSITIVITIES")
            .use_delimiter(true)
            .help("Comma-separated list of sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.")
            .takes_value(true)
        )
        .get_matches();

    let input_audio_path = PathBuf::from(matches.value_of("input_audio_path").unwrap());

    let keywords_or_paths: KeywordsOrPaths = {
        if matches.is_present("keyword_paths") {
            KeywordsOrPaths::KeywordPaths(
                matches
                    .values_of("keyword_paths")
                    .unwrap()
                    .map(|path| PathBuf::from(path.to_string()))
                    .collect(),
            )
        } else if matches.is_present("keywords") {
            KeywordsOrPaths::Keywords(
                matches
                    .values_of("keywords")
                    .unwrap()
                    .flat_map(|keyword| match BuiltinKeywords::from_str(keyword) {
                        Ok(keyword) => vec![keyword],
                        Err(_) => vec![],
                    })
                    .collect(),
            )
        } else {
            panic!("Keywords or keyword paths must be specified");
        }
    };

    let sensitivities: Option<Vec<f32>> = matches.values_of("sensitivities").map(|sensitivities| sensitivities
                    .map(|sensitivity| sensitivity.parse::<f32>().unwrap())
                    .collect());

    let model_path = matches.value_of("model_path");

    let access_key = matches
        .value_of("access_key")
        .expect("AccessKey is REQUIRED for Porcupine operation");

    porcupine_demo(
        input_audio_path,
        access_key,
        keywords_or_paths,
        sensitivities,
        model_path,
    );
}
