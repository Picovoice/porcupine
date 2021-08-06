/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use libc::{c_char, c_float};
use libloading::{Library, Symbol};
use std::cmp::PartialEq;
use std::convert::AsRef;
use std::ffi::CString;
use std::path::Path;
use std::ptr::addr_of_mut;
use std::sync::Arc;

#[repr(C)]
struct CPorcupine {}

#[repr(C)]
#[derive(PartialEq, Debug)]
#[allow(non_camel_case_types)]
pub enum PicovoiceStatuses {
    SUCCESS = 0,
    OUT_OF_MEMORY = 1,
    IO_ERROR = 2,
    INVALID_ARGUMENT = 3,
    STOP_ITERATION = 4,
    KEY_ERROR = 5,
    INVALID_STATE = 6,
}

type PvPorcupineInitFn<'a> = Symbol<
    'a,
    unsafe extern "C" fn(
        model_path: *const c_char,
        num_keywords: i32,
        keyword_paths: *const *const c_char,
        sensitivities: *const c_float,
        object: *mut *mut CPorcupine,
    ) -> PicovoiceStatuses,
>;
type PvSampleRateFn<'a> = Symbol<'a, unsafe extern "C" fn() -> i32>;
type PvPorcupineFrameLengthFn<'a> = Symbol<'a, unsafe extern "C" fn() -> i32>;
type PvPorcupineProcessFn<'a> =
    Symbol<'a, unsafe extern "C" fn(*mut CPorcupine, *const i16, *mut i32) -> PicovoiceStatuses>;
type PvPorcupineDeleteFn<'a> = Symbol<'a, unsafe extern "C" fn(*mut CPorcupine)>;

#[derive(Debug)]
pub enum PorcupineErrorStatus {
    LibraryError(PicovoiceStatuses),
    FrameLengthError,
    ArgumentError,
}

#[derive(Debug)]
pub struct PorcupineError {
    pub status: PorcupineErrorStatus,
    pub message: Option<String>,
}

impl PorcupineError {
    pub fn new(status: PorcupineErrorStatus, message: &str) -> Self {
        PorcupineError {
            status,
            message: Some(message.to_string()),
        }
    }
}

impl std::fmt::Display for PorcupineError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match &self.message {
            Some(message) => write!(f, "{}: {:?}", message, self.status),
            None => write!(f, "Porcupine error: {:?}", self.status),
        }
    }
}

#[derive(Clone)]
pub struct Porcupine {
    inner: Arc<PorcupineInner>,
}

impl Porcupine {
    pub fn new<P: AsRef<Path>>(
        library_path: P,
        model_path: P,
        keyword_paths: &[P],
        sensitivities: &[f32],
    ) -> Result<Self, PorcupineError> {
        let inner = PorcupineInner::new(library_path, model_path, keyword_paths, sensitivities);
        return match inner {
            Ok(inner) => Ok(Porcupine {
                inner: Arc::new(inner),
            }),
            Err(err) => Err(err),
        };
    }

    pub fn process(&self, pcm: &[i16]) -> Result<i32, PorcupineError> {
        return self.inner.process(pcm);
    }

    pub fn frame_length(&self) -> u32 {
        return self.inner.frame_length as u32;
    }

    pub fn sample_rate(&self) -> u32 {
        return self.inner.sample_rate as u32;
    }
}

struct PorcupineInner {
    cporcupine: *mut CPorcupine,
    lib: Library,
    frame_length: i32,
    sample_rate: i32,
}

fn pathbuf_to_cstring<P: AsRef<Path>>(pathbuf: P) -> CString {
    let pathstr = pathbuf.as_ref().to_str().unwrap();
    return CString::new(pathstr).unwrap();
}

impl PorcupineInner {
    pub fn new<P: AsRef<Path>>(
        library_path: P,
        model_path: P,
        keyword_paths: &[P],
        sensitivities: &[f32],
    ) -> Result<Self, PorcupineError> {
        unsafe {
            if !library_path.as_ref().exists() {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    &format!(
                        "Couldn't find Porcupine's dynamic library at {}",
                        library_path.as_ref().display()
                    ),
                ));
            }

            if !model_path.as_ref().exists() {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    &format!(
                        "Couldn't find model file at {}",
                        model_path.as_ref().display()
                    ),
                ));
            }

            if keyword_paths.len() != sensitivities.len() {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    "Number of keywords does not match the number of sensitivities",
                ));
            }

            for keyword_path in keyword_paths {
                if !keyword_path.as_ref().exists() {
                    return Err(PorcupineError::new(
                        PorcupineErrorStatus::ArgumentError,
                        &format!(
                            "Couldn't find keyword file at {}",
                            keyword_path.as_ref().display()
                        ),
                    ));
                }
            }

            for sensitivity in sensitivities {
                if !(0.0 <= *sensitivity && *sensitivity <= 1.0) {
                    return Err(PorcupineError::new(
                        PorcupineErrorStatus::ArgumentError,
                        "A sensitivity value should be within [0, 1]",
                    ));
                }
            }

            let lib = Library::new(library_path.as_ref())
                .expect("Failed to load porcupine dynamic library");

            let pv_porcupine_init: PvPorcupineInitFn = lib
                .get(b"pv_porcupine_init")
                .expect("Failed to load init function symbol from Porcupine library");

            let pv_model_path = pathbuf_to_cstring(&model_path);
            let pv_keyword_paths = keyword_paths
                .iter()
                .map(|keyword_path| pathbuf_to_cstring(keyword_path))
                .collect::<Vec<_>>();
            let pv_keyword_paths_ptrs = pv_keyword_paths
                .iter()
                .map(|keyword_path| keyword_path.as_ptr())
                .collect::<Vec<_>>();
            let mut cporcupine = std::ptr::null_mut();

            let status = pv_porcupine_init(
                pv_model_path.as_ptr(),
                pv_keyword_paths.len() as i32,
                pv_keyword_paths_ptrs.as_ptr(),
                sensitivities.as_ptr(),
                addr_of_mut!(cporcupine),
            );
            if status != PicovoiceStatuses::SUCCESS {
                panic!(
                    "{}",
                    PorcupineError::new(
                        PorcupineErrorStatus::LibraryError(status),
                        "Failed to initialize the Porcupine library",
                    )
                );
            }

            let pv_sample_rate: PvSampleRateFn = lib
                .get(b"pv_sample_rate")
                .expect("Failed to load function symbol from Porcupine library");

            let pv_porcupine_frame_length: PvPorcupineFrameLengthFn = lib
                .get(b"pv_porcupine_frame_length")
                .expect("Failed to load function symbol from Porcupine library");

            let sample_rate = pv_sample_rate();
            let frame_length = pv_porcupine_frame_length();

            return Ok(Self {
                cporcupine,
                lib,
                sample_rate,
                frame_length,
            });
        }
    }

    pub fn process(&self, pcm: &[i16]) -> Result<i32, PorcupineError> {
        if pcm.len() as i32 != self.frame_length {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::FrameLengthError,
                &format!(
                    "Found a frame length of {} Expected {}",
                    pcm.len(),
                    self.frame_length
                ),
            ));
        }

        let mut result: i32 = -1;
        let status = unsafe {
            let pv_porcupine_process: PvPorcupineProcessFn = self
                .lib
                .get(b"pv_porcupine_process")
                .expect("Failed to load function symbol from Porcupine library");

            pv_porcupine_process(self.cporcupine, pcm.as_ptr(), addr_of_mut!(result))
        };

        if status != PicovoiceStatuses::SUCCESS {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::LibraryError(status),
                "Porcupine library failed to process",
            ));
        }

        return Ok(result);
    }
}

unsafe impl Send for PorcupineInner {}
unsafe impl Sync for PorcupineInner {}

impl Drop for PorcupineInner {
    fn drop(&mut self) {
        unsafe {
            let pv_porcupine_delete: PvPorcupineDeleteFn = self
                .lib
                .get(b"pv_porcupine_delete")
                .expect("Failed to load function symbol from Porcupine library");
            pv_porcupine_delete(self.cporcupine);
        }
    }
}

#[cfg(test)]
mod tests {
    use itertools::Itertools;
    use rodio::{source::Source, Decoder};
    use std::fs::File;
    use std::io::BufReader;
    use std::path::PathBuf;

    use crate::porcupine::Porcupine;
    use crate::util::{pv_keyword_paths, pv_library_path, pv_model_path};

    #[test]
    fn test_process() {
        let basedir_path = "../../";
        let porcupine = Porcupine::new(
            pv_library_path(basedir_path.clone()),
            pv_model_path(basedir_path.clone()),
            &[PathBuf::from(
                pv_keyword_paths(basedir_path.clone())
                    .get("porcupine")
                    .unwrap(),
            )],
            &[0.5],
        )
        .expect("Unable to create Porcupine");

        let soundfile =
            BufReader::new(File::open("../../resources/audio_samples/porcupine.wav").unwrap());
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
        let basedir_path = "../../";
        const KEYWORDS: [&str; 8] = [
            "americano",
            "blueberry",
            "bumblebee",
            "grapefruit",
            "grasshopper",
            "picovoice",
            "porcupine",
            "terminator",
        ];

        let keyword_paths = pv_keyword_paths(basedir_path.clone());
        let selected_keyword_paths = KEYWORDS
            .iter()
            .map(|keyword| PathBuf::from(keyword_paths.get(&keyword.to_string()).unwrap()))
            .collect::<Vec<_>>();

        let porcupine = Porcupine::new(
            pv_library_path(basedir_path.clone()),
            pv_model_path(basedir_path.clone()),
            &selected_keyword_paths,
            &[0.5; KEYWORDS.len()],
        )
        .expect("Unable to create Porcupine");

        let soundfile = BufReader::new(
            File::open("../../resources/audio_samples/multiple_keywords.wav").unwrap(),
        );
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

        assert_eq!(results, [6, 0, 1, 2, 3, 4, 5, 6, 7]);
    }
}
