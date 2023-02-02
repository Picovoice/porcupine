/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use chrono::prelude::*;
use clap::{App, Arg, ArgGroup};
use porcupine::{BuiltinKeywords, PorcupineBuilder};
use pv_recorder::RecorderBuilder;
use std::path::PathBuf;
use std::str::FromStr;
use std::sync::atomic::{AtomicBool, Ordering};

static LISTENING: AtomicBool = AtomicBool::new(false);

fn porcupine_demo(
    audio_device_index: i32,
    access_key: &str,
    keywords_or_paths: KeywordsOrPaths,
    sensitivities: Option<Vec<f32>>,
    model_path: Option<&str>,
    output_path: Option<&str>,
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

    let recorder = RecorderBuilder::new()
        .device_index(audio_device_index)
        .frame_length(porcupine.frame_length() as i32)
        .init()
        .expect("Failed to initialize pvrecorder");
    recorder.start().expect("Failed to start audio recording");

    LISTENING.store(true, Ordering::SeqCst);
    ctrlc::set_handler(|| {
        LISTENING.store(false, Ordering::SeqCst);
    })
    .expect("Unable to setup signal handler");

    println!("Listening for wake words...");

    let mut audio_data = Vec::new();
    while LISTENING.load(Ordering::SeqCst) {
        let mut pcm = vec![0; recorder.frame_length()];
        recorder.read(&mut pcm).expect("Failed to read audio frame");

        let keyword_index = porcupine.process(&pcm).unwrap();
        if keyword_index >= 0 {
            println!(
                "[{}] Detected {}",
                Local::now().format("%F %T"),
                keywords_or_paths.get(keyword_index as usize)
            );
        }

        if output_path.is_some() {
            audio_data.extend_from_slice(&pcm);
        }
    }

    println!("\nStopping...");
    recorder.stop().expect("Failed to stop audio recording");

    if let Some(output_path) = output_path {
        let wavspec = hound::WavSpec {
            channels: 1,
            sample_rate: porcupine.sample_rate(),
            bits_per_sample: 16,
            sample_format: hound::SampleFormat::Int,
        };
        let mut writer = hound::WavWriter::create(output_path, wavspec)
            .expect("Failed to open output audio file");
        for sample in audio_data {
            writer.write_sample(sample).unwrap();
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

fn show_audio_devices() {
    let audio_devices = RecorderBuilder::new()
        .init()
        .expect("Failed to initialize pvrecorder")
        .get_audio_devices();
    match audio_devices {
        Ok(audio_devices) => {
            for (idx, device) in audio_devices.iter().enumerate() {
                println!("index: {idx}, device name: {device:?}");
            }
        }
        Err(err) => panic!("Failed to get audio devices: {}", err),
    };
}

fn main() {
    let matches = App::new("Picovoice Porcupine Rust Mic Demo")
        .group(
            ArgGroup::with_name("actions_group")
            .arg("access_key")
            .arg("keywords")
            .arg("keyword_paths")
            .arg("show_audio_devices")
            .required(true)
            .multiple(true)
        )
        .arg(
            Arg::with_name("access_key")
                .long("access_key")
                .value_name("ACCESS_KEY")
                .help("AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)")
                .takes_value(true)
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
        .arg(
            Arg::with_name("audio_device_index")
            .long("audio_device_index")
            .value_name("INDEX")
            .help("Index of input audio device.")
            .takes_value(true)
            .default_value("-1")
        )
        .arg(
            Arg::with_name("output_path")
            .long("output_path")
            .value_name("PATH")
            .help("Path to recorded audio (for debugging).")
            .takes_value(true)
        )
        .arg(
            Arg::with_name("show_audio_devices")
            .long("show_audio_devices")
        )
        .get_matches();

    if matches.is_present("show_audio_devices") {
        return show_audio_devices();
    }

    let audio_device_index = matches
        .value_of("audio_device_index")
        .unwrap()
        .parse()
        .unwrap();

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
    let output_path = matches.value_of("output_path");

    let access_key = matches
        .value_of("access_key")
        .expect("AccessKey is REQUIRED for Porcupine operation");

    porcupine_demo(
        audio_device_index,
        access_key,
        keywords_or_paths,
        sensitivities,
        model_path,
        output_path,
    );
}
