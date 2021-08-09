/*
    Copyright 2021-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use chrono::prelude::*;
use clap::{App, Arg, ArgGroup};
use ctrlc;
use miniaudio;
use porcupine::{BuiltinKeywords, Porcupine, PorcupineBuilder};
use std::path::PathBuf;
use std::sync::atomic::{AtomicBool, Ordering};

static LISTENING: AtomicBool = AtomicBool::new(false);

fn process_audio_chunk(
    samples: &[i16],
    porcupine: &Porcupine,
    buffer: &mut Vec<i16>,
    keywords_or_paths: &KeywordsOrPaths,
) {
    buffer.extend_from_slice(samples);

    while buffer.len() >= porcupine.frame_length() as usize && LISTENING.load(Ordering::SeqCst) {
        let frame: Vec<i16> = buffer.drain(..porcupine.frame_length() as usize).collect();
        let keyword_index = porcupine.process(&frame).unwrap();
        if keyword_index >= 0 {
            println!(
                "[{}] Detected {}",
                Local::now(),
                keywords_or_paths.get(keyword_index as usize)
            );
        }
    }
}

fn porcupine_demo(
    miniaudio_backend: &[miniaudio::Backend],
    audio_device_index: usize,
    keywords_or_paths: KeywordsOrPaths,
    sensitivities: Option<Vec<f32>>,
    model_path: Option<&str>,
) {
    let mut buffer: Vec<i16> = Vec::new();

    let mut porcupine_builder = match keywords_or_paths {
        KeywordsOrPaths::Keywords(ref keywords) => PorcupineBuilder::new_with_keywords(&keywords),
        KeywordsOrPaths::KeywordPaths(ref keyword_paths) => {
            PorcupineBuilder::new_with_keyword_paths(&keyword_paths)
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

    let miniaudio_context =
        miniaudio::Context::new(&miniaudio_backend, None).expect("Failed to create audio context");
    miniaudio_context
        .with_capture_devices(|_: _| {})
        .expect("Failed to access capture devices");
    let device_id = miniaudio_context
        .capture_devices()
        .get(audio_device_index)
        .expect("No device available given audio device index.")
        .id()
        .clone();

    println!("Using {:?} backend", miniaudio_context.backend());

    let mut device_config = miniaudio::DeviceConfig::new(miniaudio::DeviceType::Capture);
    device_config
        .capture_mut()
        .set_format(miniaudio::Format::S16);
    device_config.capture_mut().set_channels(1);
    device_config.capture_mut().set_device_id(Some(device_id));
    device_config.set_sample_rate(porcupine.sample_rate());
    device_config.set_data_callback(move |_, _, frames| {
        process_audio_chunk(
            frames.as_samples(),
            &porcupine,
            &mut buffer,
            &keywords_or_paths,
        );
    });

    let device = miniaudio::Device::new(Some(miniaudio_context), &device_config)
        .expect("Failed to initialize capture device");

    LISTENING.store(true, Ordering::SeqCst);
    device.start().expect("Failed to start device");
    println!("Listening for wake words...");

    ctrlc::set_handler(|| {
        LISTENING.store(false, Ordering::SeqCst);
    })
    .expect("Unable to setup signal handler");

    // Spin loop until we receive the ctrlc handler
    while LISTENING.load(Ordering::SeqCst) {
        std::hint::spin_loop();
        std::thread::sleep(std::time::Duration::from_millis(10));
    }

    println!("\nStopping!");
    device.stop().expect("Failed to stop device");
    println!("Stopped");
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

fn show_audio_devices(miniaudio_backend: &[miniaudio::Backend]) {
    let miniaudio_context =
        miniaudio::Context::new(miniaudio_backend, None).expect("failed to create context");
    miniaudio_context
        .with_capture_devices(|capture_devices| {
            println!("Capture Devices:");
            for (idx, device) in capture_devices.iter().enumerate() {
                println!("\t{}: {}", idx, device.name());
            }
        })
        .expect("failed to get devices");
}

fn main() {
    let matches = App::new("Picovoice Porcupine Rust Mic Demo")
        .group(
            ArgGroup::with_name("keywords_group")
            .arg("keywords")
            .arg("keyword_paths")
            .arg("show_audio_devices")
            .required(true)
        )
        .arg(
            Arg::with_name("keywords")
            .long("keywords")
            .value_name("KEYWORDS")
            .use_delimiter(true)
            .help("Comma-seperated list of default keywords for detection.")
            .takes_value(true)
            .possible_values(&BuiltinKeywords::options())
        )
        .arg(
            Arg::with_name("keyword_paths")
            .long("keyword_paths")
            .value_name("PATHS")
            .use_delimiter(true)
            .help("Comma-seperated list of paths to keyword model files. If not set it will be populated from `--keywords` argument.")
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
            .help("Comma-seperated list of sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.")
            .takes_value(true)
        )
        .arg(
            Arg::with_name("audio_device_index")
            .long("audio_device_index")
            .value_name("INDEX")
            .help("Index of input audio device.")
            .takes_value(true)
            .default_value("0")
        )
        .arg(
            Arg::with_name("audio_backend")
            .long("audio_backend")
            .value_name("BACKEND")
            .help("The name of a specific audio backend to use. Note: not all options will work on a given platform.")
            .takes_value(true)
            .possible_values(&["Wasapi", "DSound", "WinMM", "CoreAudio", "SNDIO", "Audio4", "OSS", "PulseAudio", "Alsa", "Jack", "AAudio", "OpenSL", "WebAudio"])
        )
        .arg(
            Arg::with_name("show_audio_devices")
            .long("show_audio_devices")
        )
        .get_matches();

    let miniaudio_backend = match matches.value_of("audio_backend") {
        Some(audio_backend_str) => vec![match audio_backend_str {
            "Wasapi" => miniaudio::Backend::Wasapi,
            "DSound" => miniaudio::Backend::DSound,
            "WinMM" => miniaudio::Backend::WinMM,
            "CoreAudio" => miniaudio::Backend::CoreAudio,
            "SNDIO" => miniaudio::Backend::SNDIO,
            "Audio4" => miniaudio::Backend::Audio4,
            "OSS" => miniaudio::Backend::OSS,
            "PulseAudio" => miniaudio::Backend::PulseAudio,
            "Alsa" => miniaudio::Backend::Alsa,
            "Jack" => miniaudio::Backend::Jack,
            "AAudio" => miniaudio::Backend::AAudio,
            "OpenSL" => miniaudio::Backend::OpenSL,
            "WebAudio" => miniaudio::Backend::WebAudio,
            _ => panic!("Unsupported audio backend"),
        }],
        _ => vec![],
    };

    if matches.is_present("show_audio_devices") {
        return show_audio_devices(&miniaudio_backend);
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
                        Some(keyword) => vec![keyword],
                        None => vec![],
                    })
                    .collect(),
            )
        } else {
            panic!("Keywords or keyword paths must be specified");
        }
    };

    let sensitivities: Option<Vec<f32>> = match matches.values_of("sensitivities") {
        Some(sensitivities) => Some(
            sensitivities
                .map(|sensitivity| sensitivity.parse::<f32>().unwrap())
                .collect(),
        ),
        None => None,
    };

    let model_path = matches.value_of("model_path");

    porcupine_demo(
        &miniaudio_backend,
        audio_device_index,
        keywords_or_paths,
        sensitivities,
        model_path,
    );
}
