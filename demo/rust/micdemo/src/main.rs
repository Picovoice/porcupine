/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use clap::{App, Arg, ArgGroup};
use ctrlc;
use miniaudio;
use pv_porcupine::{pv_keyword_paths, pv_library_path, pv_model_path, Porcupine, KEYWORDS};
use std::path::PathBuf;
use std::sync::atomic::{AtomicBool, Ordering};
use std::time::SystemTime;

static LISTENING: AtomicBool = AtomicBool::new(true);

fn process_audio_chunk(
    samples: &[i16],
    porcupine: &mut Porcupine,
    buffer: &mut Vec<i16>,
    keywords: &[String],
) {
    buffer.extend_from_slice(samples);

    while buffer.len() >= porcupine.frame_length() as usize && LISTENING.load(Ordering::SeqCst) {
        let frame: Vec<i16> = buffer.drain(..porcupine.frame_length() as usize).collect();
        let result = porcupine.process(&frame);
        if result >= 0 {
            println!(
                "[{}] Detected {}",
                SystemTime::now()
                    .duration_since(SystemTime::UNIX_EPOCH)
                    .unwrap()
                    .as_secs(),
                keywords[result as usize]
            );
        }
    }
}

fn porcupine_demo(
    audio_backend: Option<miniaudio::Backend>,
    audio_device_index: usize,
    library_path: PathBuf,
    model_path: PathBuf,
    keyword_paths: Vec<PathBuf>,
    keywords: Vec<String>,
    sensitivities: Vec<f32>,
) {
    let mut buffer: Vec<i16> = Vec::new();
    let mut porcupine = Porcupine::new(library_path, model_path, &keyword_paths, &sensitivities);

    let miniaudio_backend = match audio_backend {
        Some(backend) => vec![backend],
        _ => vec![],
    };
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
        process_audio_chunk(frames.as_samples(), &mut porcupine, &mut buffer, &keywords);
    });

    let device = miniaudio::Device::new(Some(miniaudio_context), &device_config)
        .expect("Failed to initialize capture device");
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
}

fn show_audio_devices() {
    let miniaudio_context = miniaudio::Context::new(&[], None).expect("failed to create context");
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
    let default_library_path = pv_library_path(PathBuf::from("../../../"))
        .into_os_string()
        .into_string()
        .unwrap();
    let default_model_path = pv_model_path(PathBuf::from("../../../"))
        .into_os_string()
        .into_string()
        .unwrap();

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
            .help("List of default keywords for detection.")
            .takes_value(true)
            .possible_values(&KEYWORDS)
        )
        .arg(
            Arg::with_name("keyword_paths")
            .long("keyword_paths")
            .value_name("PATHS")
            .use_delimiter(true)
            .help("Absolute paths to keyword model files. If not set it will be populated from `--keywords` argument.")
            .takes_value(true)
        )
        .arg(
            Arg::with_name("library_path")
            .long("library_path")
            .value_name("PATH")
            .help("Absolute path to dynamic library.")
            .takes_value(true)
            .default_value(&default_library_path)
        )
        .arg(
            Arg::with_name("model_path")
            .long("model_path")
            .value_name("PATH")
            .help("Absolute path to the file containing model parameter.")
            .takes_value(true)
            .default_value(&default_model_path)
        )
        .arg(
            Arg::with_name("sensitivities")
            .long("sensitivities")
            .value_name("SENSITIVITIES")
            .use_delimiter(true)
            .help("Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.")
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

    if matches.is_present("show_audio_devices") {
        show_audio_devices();
    } else {
        let audio_device_index = matches
            .value_of("audio_device_index")
            .unwrap()
            .parse()
            .unwrap();

        let audio_backend = match matches.value_of("audio_backend") {
            Some(audio_backend_str) => Some(match audio_backend_str {
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
            }),
            _ => None,
        };

        let library_path = PathBuf::from(matches.value_of("library_path").unwrap());
        let model_path = PathBuf::from(matches.value_of("model_path").unwrap());

        let keyword_paths: Vec<PathBuf> = {
            if matches.is_present("keywords") {
                let pv_keyword_paths = pv_keyword_paths(PathBuf::from("../../../"));
                matches
                    .values_of("keywords")
                    .unwrap()
                    .map(|keyword| {
                        PathBuf::from(pv_keyword_paths.get(&keyword.to_string()).unwrap().clone())
                    })
                    .collect()
            } else {
                matches
                    .values_of("keyword_paths")
                    .unwrap()
                    .map(|path| PathBuf::from(path.to_string()))
                    .collect()
            }
        };

        let keywords: Vec<String> = keyword_paths
            .iter()
            .map(|path| {
                path.file_name()
                    .unwrap()
                    .to_os_string()
                    .into_string()
                    .unwrap()
                    .split("_")
                    .next()
                    .unwrap()
                    .to_string()
            })
            .collect();

        println!("{:?}", keywords);

        let sensitivities: Vec<f32> = {
            if matches.is_present("sensitivities") {
                matches
                    .values_of("sensitivities")
                    .unwrap()
                    .map(|sensitivity| sensitivity.parse::<f32>().unwrap())
                    .collect()
            } else {
                let mut sensitivities = Vec::new();
                sensitivities.resize_with(keyword_paths.len(), || 0.5);
                sensitivities
            }
        };

        if keyword_paths.len() != sensitivities.len() {
            panic!("Number of keywords does not match the number of sensitivities.");
        }

        porcupine_demo(
            audio_backend,
            audio_device_index,
            library_path,
            model_path,
            keyword_paths,
            keywords,
            sensitivities,
        );
    }
}
