/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

mod platform {
    #[allow(dead_code)]
    const RPI_MACHINES: [&str; 4] = ["arm11", "cortex-a7", "cortex-a53", "cortex-a72"];
    #[allow(dead_code)]
    const JETSON_MACHINES: [&str; 1] = ["cortex-a57"];

    #[cfg(target_os = "macos")]
    pub fn pv_platform() -> String {
        return String::from("mac");
    }

    #[cfg(target_os = "windows")]
    pub fn pv_platform() -> String {
        return String::from("windows");
    }

    #[cfg(all(target_os = "linux", target_arch = "x86_64"))]
    pub fn pv_platform() -> String {
        return String::from("linux");
    }

    #[cfg(all(target_os = "linux", any(target_arch = "arm", target_arch = "aarch64")))]
    pub fn pv_platform() -> String {
        let machine = find_machine_type();
        return match machine.as_str() {
            machine if RPI_MACHINES.contains(&machine) => String::from("raspberry-pi"),
            machine if JETSON_MACHINES.contains(&machine) => String::from("jetson"),
            "beaglebone" => String::from("beaglebone"),
            _ => {
                panic!("ERROR: Please be advised that this device is not officially supported by Picovoice");
            }
        };
    }
}

#[cfg(test)]
mod tests {
    use itertools::Itertools;
    use rodio::{source::Source, Decoder};
    use std::env;
    use std::fs::File;
    use std::io::BufReader;

    use super::platform::pv_platform;
    use porcupine::{BuiltinKeywords, PorcupineBuilder};

    #[test]
    fn test_process() {
        let access_key = env::var("PV_ACCESS_KEY")
            .expect("Pass the AccessKey in using the PV_ACCESS_KEY env variable");
        let porcupine =
            PorcupineBuilder::new_with_keywords(access_key, &[BuiltinKeywords::Porcupine])
                .init()
                .expect("Unable to create Porcupine");

        let soundfile_path = concat!(
            env!("CARGO_MANIFEST_DIR"),
            "/../../resources/audio_samples/porcupine.wav"
        );
        let soundfile = BufReader::new(File::open(soundfile_path).expect(soundfile_path));
        let source = Decoder::new(soundfile).unwrap();

        assert_eq!(porcupine.sample_rate(), source.sample_rate());

        let mut results = Vec::new();
        for frame in &source.chunks(porcupine.frame_length() as usize) {
            let frame = frame.collect_vec();
            if frame.len() == porcupine.frame_length() as usize {
                let keyword_index = porcupine.process(&frame).unwrap();
                if keyword_index >= 0 {
                    results.push(keyword_index);
                }
            }
        }

        assert_eq!(results, [0]);
    }

    #[test]
    fn test_process_multiple() {
        let selected_keywords = vec![
            BuiltinKeywords::Americano,
            BuiltinKeywords::Blueberry,
            BuiltinKeywords::Bumblebee,
            BuiltinKeywords::Grapefruit,
            BuiltinKeywords::Grasshopper,
            BuiltinKeywords::Picovoice,
            BuiltinKeywords::Porcupine,
            BuiltinKeywords::Terminator,
        ];

        let access_key = env::var("PV_ACCESS_KEY")
            .expect("Pass the AccessKey in using the PV_ACCESS_KEY env variable");
        let porcupine = PorcupineBuilder::new_with_keywords(access_key, &selected_keywords)
            .init()
            .expect("Unable to create Porcupine");

        let soundfile_path = concat!(
            env!("CARGO_MANIFEST_DIR"),
            "/../../resources/audio_samples/multiple_keywords.wav"
        );
        let soundfile = BufReader::new(File::open(soundfile_path).expect(soundfile_path));
        let source = Decoder::new(soundfile).unwrap();

        assert_eq!(porcupine.sample_rate(), source.sample_rate());

        let mut results = Vec::new();
        for frame in &source.chunks(porcupine.frame_length() as usize) {
            let frame = frame.collect_vec();
            if frame.len() == porcupine.frame_length() as usize {
                let keyword_index = porcupine.process(&frame).unwrap();
                if keyword_index >= 0 {
                    results.push(keyword_index);
                }
            }
        }

        let expected_results = vec![
            BuiltinKeywords::Porcupine,
            BuiltinKeywords::Americano,
            BuiltinKeywords::Blueberry,
            BuiltinKeywords::Bumblebee,
            BuiltinKeywords::Grapefruit,
            BuiltinKeywords::Grasshopper,
            BuiltinKeywords::Picovoice,
            BuiltinKeywords::Porcupine,
            BuiltinKeywords::Terminator,
        ];

        assert_eq!(results.len(), expected_results.len());

        for (result, expected_result) in results.iter().zip(expected_results) {
            assert_eq!(
                *selected_keywords.get(*result as usize).unwrap(),
                expected_result
            );
        }
    }

    #[test]
    fn test_with_non_ascii_model_name() {
        let access_key = env::var("PV_ACCESS_KEY")
            .expect("Pass the AccessKey in using the PV_ACCESS_KEY env variable");
        let keyword_path = format!(
            "../../resources/keyword_files_es/{}/murciélago_{}.ppn",
            pv_platform(),
            pv_platform(),
        );
        let porcupine = PorcupineBuilder::new_with_keyword_paths(
            access_key, 
            &[keyword_path])
        .model_path("../../lib/common/porcupine_params_es.pv")
        .init()
        .expect("Unable to create Porcupine");

        let soundfile_path = concat!(
            env!("CARGO_MANIFEST_DIR"),
            "/../../resources/audio_samples/murciélago.wav"
        );

        let soundfile = BufReader::new(File::open(soundfile_path).expect(soundfile_path));
        let source = Decoder::new(soundfile).unwrap();

        assert_eq!(porcupine.sample_rate(), source.sample_rate());

        let mut results = Vec::new();
        for frame in &source.chunks(porcupine.frame_length() as usize) {
            let frame = frame.collect_vec();
            if frame.len() == porcupine.frame_length() as usize {
                let keyword_index = porcupine.process(&frame).unwrap();
                if keyword_index >= 0 {
                    results.push(keyword_index);
                }
            }
        }

        let expected_results = vec![0, 0];

        assert_eq!(results, expected_results);
    }
}
