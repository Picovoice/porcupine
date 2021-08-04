use libc::{c_char, c_float};
use libloading::{Library, Symbol};
use std::cmp::PartialEq;
use std::ffi::CString;
use std::path::PathBuf;
use std::ptr::addr_of_mut;

#[repr(C)]
struct CPorcupine {}

#[repr(C)]
#[derive(PartialEq)]
#[allow(non_camel_case_types)]
enum PicovoiceStatuses {
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

pub struct Porcupine {
    cporcupine: *mut CPorcupine,
    lib: Library,
    pub frame_length: i32,
    pub sample_rate: i32,
}

fn pathbuf_to_cstring(pathbuf: &PathBuf) -> CString {
    let pathstr = pathbuf.to_str().unwrap();
    return CString::new(pathstr).unwrap();
}

impl Porcupine {
    pub fn new(
        library_path: PathBuf,
        model_path: PathBuf,
        keyword_paths: &[PathBuf],
        sensitivities: &[f32],
    ) -> Self {
        unsafe {
            let lib = Library::new(library_path).expect("Failed to load porcupine dynamic library");

            let pv_porcupine_init: PvPorcupineInitFn = lib
                .get(b"pv_porcupine_init")
                .expect("Failed to load init function");

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
                panic!("Make this an actual raised error");
            }

            let pv_sample_rate: PvSampleRateFn =
                lib.get(b"pv_sample_rate").expect("Failed to load function");

            let pv_porcupine_frame_length: PvPorcupineFrameLengthFn = lib
                .get(b"pv_porcupine_frame_length")
                .expect("Failed to load function");

            let sample_rate = pv_sample_rate();
            let frame_length = pv_porcupine_frame_length();

            return Porcupine {
                cporcupine,
                lib,
                sample_rate,
                frame_length,
            };
        }
    }

    pub fn process(&mut self, pcm: &[i16]) -> i32 {
        if pcm.len() as i32 != self.frame_length {
            panic!(
                "Make this an actual raised error: frame length bad: {}/{}",
                pcm.len(),
                self.frame_length
            );
        }

        let mut result: i32 = 0;
        let status = unsafe {
            let pv_porcupine_process: PvPorcupineProcessFn = self
                .lib
                .get(b"pv_porcupine_process")
                .expect("Failed to load function");

            pv_porcupine_process(self.cporcupine, pcm.as_ptr(), addr_of_mut!(result))
        };

        if status != PicovoiceStatuses::SUCCESS {
            panic!("Make this an actual raised error");
        }

        return result;
    }
}

impl Drop for Porcupine {
    fn drop(&mut self) {
        unsafe {
            let pv_porcupine_delete: PvPorcupineDeleteFn = self
                .lib
                .get(b"pv_porcupine_delete")
                .expect("Failed to load function");
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

    use crate::Porcupine;

    #[test]
    fn test_process() {
        let mut porcupine = Porcupine::new(
            PathBuf::from("../../lib/linux/x86_64/libpv_porcupine.so"),
            PathBuf::from("../../lib/common/porcupine_params.pv"),
            &[PathBuf::from(
                "../../resources/keyword_files/linux/porcupine_linux.ppn",
            )],
            &[0.5],
        );

        let soundfile =
            BufReader::new(File::open("../../resources/audio_samples/porcupine.wav").unwrap());
        let source = Decoder::new(soundfile).unwrap();

        assert_eq!(porcupine.sample_rate, source.sample_rate() as i32);

        let mut results = Vec::new();
        for frame in &source.chunks(porcupine.frame_length as usize) {
            let frame = frame.collect_vec();
            if frame.len() == porcupine.frame_length as usize {
                let keyword_index = porcupine.process(&frame);
                if keyword_index >= 0 {
                    results.push(keyword_index);
                }
            }
        }

        assert_eq!(results, [0]);
    }

    #[test]
    fn test_process_multiple() {
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

        let keyword_paths = KEYWORDS
            .iter()
            .map(|keyword| {
                PathBuf::from(format!(
                    "../../resources/keyword_files/linux/{}_linux.ppn",
                    keyword
                ))
            })
            .collect::<Vec<_>>();

        let mut porcupine = Porcupine::new(
            PathBuf::from("../../lib/linux/x86_64/libpv_porcupine.so"),
            PathBuf::from("../../lib/common/porcupine_params.pv"),
            &keyword_paths,
            &[0.5; KEYWORDS.len()],
        );

        let soundfile = BufReader::new(
            File::open("../../resources/audio_samples/multiple_keywords.wav").unwrap(),
        );
        let source = Decoder::new(soundfile).unwrap();

        assert_eq!(porcupine.sample_rate, source.sample_rate() as i32);

        let mut results = Vec::new();
        for frame in &source.chunks(porcupine.frame_length as usize) {
            let frame = frame.collect_vec();
            if frame.len() == porcupine.frame_length as usize {
                let keyword_index = porcupine.process(&frame);
                if keyword_index >= 0 {
                    results.push(keyword_index);
                }
            }
        }

        assert_eq!(results, [6, 0, 1, 2, 3, 4, 5, 6, 7]);
    }
}
