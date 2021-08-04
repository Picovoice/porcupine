use clap::{App, Arg, ArgGroup};
use itertools::Itertools;
use pv_porcupine::{pv_keyword_paths, pv_library_path, pv_model_path, Porcupine, KEYWORDS};
use rodio::{source::Source, Decoder};
use std::fs::File;
use std::io::BufReader;
use std::path::PathBuf;
use std::time::SystemTime;

fn porcupine_demo(
    input_audio_path: PathBuf,
    library_path: PathBuf,
    model_path: PathBuf,
    keyword_paths: Vec<PathBuf>,
    keywords: Vec<String>,
    sensitivities: Vec<f32>,
) {
    let soundfile = BufReader::new(File::open(input_audio_path).unwrap());
    let audiosource = Decoder::new(soundfile).unwrap();

    let mut porcupine = Porcupine::new(library_path, model_path, &keyword_paths, &sensitivities);

    if porcupine.sample_rate != audiosource.sample_rate() as i32 {
        panic!(
            "Audio file should have the expected sample rate of {}, got {}",
            porcupine.sample_rate,
            audiosource.sample_rate()
        );
    }

    for frame in &audiosource.chunks(porcupine.frame_length as usize) {
        let frame = frame.collect_vec();
        if frame.len() == porcupine.frame_length as usize {
            let keyword_index = porcupine.process(&frame);
            if keyword_index >= 0 {
                println!(
                    "[{}] Detected {}",
                    SystemTime::now()
                        .duration_since(SystemTime::UNIX_EPOCH)
                        .unwrap()
                        .as_secs(),
                    keywords[keyword_index as usize]
                );
            }
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
        .arg(
            Arg::with_name("input_audio_path")
            .long("input_audio_path")
            .value_name("PATH")
            .help("Absolute path to input audio file.")
            .takes_value(true)
            .required(true)
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
        .get_matches();

    let input_audio_path = PathBuf::from(matches.value_of("input_audio_path").unwrap());
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
        input_audio_path,
        library_path,
        model_path,
        keyword_paths,
        keywords,
        sensitivities,
    );
}
