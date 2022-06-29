/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#[cfg(test)]
mod tests {
    use itertools::Itertools;
    use rodio::{source::Source, Decoder};
    use std::env;
    use std::fs::File;
    use std::io::BufReader;

    use porcupine::util::pv_platform;
    use porcupine::{BuiltinKeywords, PorcupineBuilder};

    fn append_lang(path: &str, language: &str) -> String {
        if language == "en" {
            String::from(path)
        } else {
            format!("{}_{}", path, language)
        }
    }

    fn model_path_by_language(language: &str) -> String {
        format!(
            "{}{}{}",
            env!("CARGO_MANIFEST_DIR"),
            append_lang("/../../lib/common/porcupine_params", language),
            ".pv"
        )
    }

    fn keyword_path_by_language(keyword_file: &str, language: &str) -> String {
        format!(
            "{}{}/{}/{}_{}.ppn",
            env!("CARGO_MANIFEST_DIR"),
            append_lang("/../../resources/keyword_files", language),
            pv_platform(),
            keyword_file,
            pv_platform()
        )
    }

    fn run_porcupine_test(
        language: &str,
        keywords: &[&str],
        ground_truth: &[i32],
        audio_file_name: &str,
    ) {
        let access_key = env::var("PV_ACCESS_KEY")
            .expect("Pass the AccessKey in using the PV_ACCESS_KEY env variable");

        let mut vec = Vec::new();
        for keyword in keywords.iter() {
            vec.push(keyword_path_by_language(keyword, language));
        }

        let porcupine = PorcupineBuilder::new_with_keyword_paths(access_key, &vec)
            .model_path(model_path_by_language(language))
            .init()
            .expect("Unable to create Porcupine");

        let soundfile_path = format!(
            "{}{}{}",
            env!("CARGO_MANIFEST_DIR"),
            "/../../resources/audio_samples/",
            audio_file_name
        );

        let soundfile = BufReader::new(File::open(&soundfile_path).expect(&soundfile_path));
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

        assert_eq!(results, ground_truth);
    }

    macro_rules! porcupine_keywords_tests {
        ($($language:ident: $values:expr,)*) => {
        $(
            #[test]
            fn $language() {
                let language = stringify!($language);
                let (keywords, ground_truth) = $values;
                let audio_file_name = format!("{}.wav", append_lang("multiple_keywords", language));
                run_porcupine_test(language, keywords, ground_truth, &audio_file_name);
            }
        )*
        }
    }

    porcupine_keywords_tests! {
        en: (
            &["americano", "blueberry","bumblebee", "grapefruit", "grasshopper", "picovoice", "porcupine", "terminator"],
            &[6, 0, 1, 2, 3, 4, 5, 6, 7],
        ),
        es: (
            &["emparedado", "leopardo", "manzana", "murciélago"],
            &[0, 1, 2, 3],
        ),
        de: (
            &["ananas", "heuschrecke", "leguan", "stachelschwein"],
            &[0, 1, 2, 3],
        ),
        fr: (
            &["framboise", "mon chouchou", "parapluie"],
            &[0, 1, 0, 2],
        ),
        ko: (
            &["aiseukeulim", "bigseubi", "koppulso"],
            &[1, 2, 0],
        ),
        ja: (
            &["ninja", "bushi", "ringo"],
            &[2, 1, 0],
        ),
        it: (
            &["espresso", "cameriere", "porcospino"],
            &[2, 0, 1],
        ),
        pt: (
            &["abacaxi", "fenomeno", "formiga"],
            &[0, 2, 1],
        ),
    }

    #[test]
    fn test_process_single_builtin() {
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
    fn test_process_multiple_builtin() {
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
}
