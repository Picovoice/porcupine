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
    use std::fs::File;
    use std::io::BufReader;

    use porcupine::{BuiltinKeywords, PorcupineBuilder};

    #[test]
    fn test_process() {
        let porcupine = PorcupineBuilder::new_with_keywords(&[BuiltinKeywords::Porcupine])
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

        let porcupine = PorcupineBuilder::new_with_keywords(&selected_keywords)
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
