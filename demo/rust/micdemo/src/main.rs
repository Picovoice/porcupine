use clap::{App, Arg, ArgGroup};
use cpal;
use cpal::traits::DeviceTrait;
use cpal::traits::HostTrait;
use cpal::traits::StreamTrait;
use ctrlc;
use pv_porcupine::{pv_keyword_paths, pv_library_path, pv_model_path, Porcupine, KEYWORDS};
use std::path::PathBuf;
use std::sync::atomic::{AtomicBool, Ordering};
use std::time::SystemTime;

static LISTENING: AtomicBool = AtomicBool::new(true);

fn process_audio_chunk(
    data: &[i16],
    porcupine: &mut Porcupine,
    buffer: &mut Vec<i16>,
    keywords: &[String],
) {
    buffer.extend_from_slice(data);

    if buffer.len() >= porcupine.frame_length as usize {
        let frame: Vec<i16> = buffer.drain(..porcupine.frame_length as usize).collect();
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
    audio_device_index: Option<usize>,
    library_path: PathBuf,
    model_path: PathBuf,
    keyword_paths: Vec<PathBuf>,
    keywords: Vec<String>,
    sensitivities: Vec<f32>,
) {
    let mut buffer: Vec<i16> = Vec::new();
    let mut porcupine = Porcupine::new(library_path, model_path, &keyword_paths, &sensitivities);

    let host = cpal::default_host();
    let device = match audio_device_index {
        Some(index) => {
            let device = host.input_devices().unwrap().enumerate().find(|(i, _device)| i == &index);
            match device {
                Some(device_tuple) => Some(device_tuple.1),
                None => None
            }
        }
        None => host.default_input_device(),
    }
    .expect("Failed to find input device");
    println!("Input device: {}", device.name().unwrap());

    let config = cpal::StreamConfig {
        channels: 1,
        sample_rate: cpal::SampleRate(porcupine.sample_rate as u32),
        buffer_size: cpal::BufferSize::Default,
    };

    println!("Listening...");
    let stream = device
        .build_input_stream(
            &config.into(),
            move |data: &[i16], _: &_| {
                process_audio_chunk(data, &mut porcupine, &mut buffer, &keywords)
            },
            |_| {},
        )
        .unwrap();
    stream.play().unwrap();

    ctrlc::set_handler(|| {
        LISTENING.store(false, Ordering::SeqCst);
    })
    .unwrap();

    // Spin loop until we receive the ctrlc handler
    while LISTENING.load(Ordering::SeqCst) {
        std::hint::spin_loop();
        std::thread::sleep(std::time::Duration::from_millis(10));
    }

    println!("Stopping!");
}

fn show_audio_devices() {
    let host = cpal::default_host();
    for (idx, device) in host.input_devices().unwrap().enumerate() {
        if let Ok(device_name) = device.name() {
            println!("Index: {}, Device: {}", idx, device_name);
        }
    }
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
        )
        .arg(
            Arg::with_name("show_audio_devices")
            .long("show_audio_devices")
        )
        .get_matches();

    if matches.is_present("show_audio_devices") {
        show_audio_devices();
    } else {
        let audio_device_index = match matches.value_of("audio_device_index") {
            Some(audio_device_index) => Some(
                audio_device_index
                    .parse::<usize>()
                    .expect("Audio device index should be a positive integer"),
            ),
            None => None,
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
            audio_device_index,
            library_path,
            model_path,
            keyword_paths,
            keywords,
            sensitivities,
        );
    }
}
