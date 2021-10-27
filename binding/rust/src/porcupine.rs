/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use lazy_static::lazy_static;
use libc::{c_char, c_float};
use libloading::{Library, Symbol};
use std::cmp::PartialEq;
use std::ffi::OsStr;
use std::ffi::{CStr, CString};
use std::path::PathBuf;
use std::ptr::addr_of_mut;
use std::sync::Arc;

use crate::util::*;

lazy_static! {
    static ref PV_PORCUPINE_LIB: Result<Library, PorcupineError> = {
        unsafe {
            match Library::new(pv_library_path()) {
                Ok(symbol) => Ok(symbol),
                Err(err) => Err(PorcupineError::new(
                    PorcupineErrorStatus::LibraryLoadError,
                    &format!("Failed to load porcupine dynamic library: {}", err),
                )),
            }
        }
    };
}

#[derive(PartialEq, Clone, Debug)]
pub enum BuiltinKeywords {
    Alexa,
    Americano,
    Blueberry,
    Bumblebee,
    Computer,
    Grapefruit,
    Grasshopper,
    HeyGoogle,
    HeySiri,
    Jarvis,
    OkGoogle,
    Picovoice,
    Porcupine,
    Terminator,
}

impl BuiltinKeywords {
    pub fn to_str(&self) -> &'static str {
        match self {
            Self::Alexa => "alexa",
            Self::Americano => "americano",
            Self::Blueberry => "blueberry",
            Self::Bumblebee => "bumblebee",
            Self::Computer => "computer",
            Self::Grapefruit => "grapefruit",
            Self::Grasshopper => "grasshopper",
            Self::HeyGoogle => "hey google",
            Self::HeySiri => "hey siri",
            Self::Jarvis => "jarvis",
            Self::OkGoogle => "ok google",
            Self::Picovoice => "picovoice",
            Self::Porcupine => "porcupine",
            Self::Terminator => "terminator",
        }
    }

    pub fn from_str(str_option: &str) -> Option<Self> {
        match str_option {
            "alexa" => Some(Self::Alexa),
            "americano" => Some(Self::Americano),
            "blueberry" => Some(Self::Blueberry),
            "bumblebee" => Some(Self::Bumblebee),
            "computer" => Some(Self::Computer),
            "grapefruit" => Some(Self::Grapefruit),
            "grasshopper" => Some(Self::Grasshopper),
            "hey google" => Some(Self::HeyGoogle),
            "hey siri" => Some(Self::HeySiri),
            "jarvis" => Some(Self::Jarvis),
            "ok google" => Some(Self::OkGoogle),
            "picovoice" => Some(Self::Picovoice),
            "porcupine" => Some(Self::Porcupine),
            "terminator" => Some(Self::Terminator),
            _ => None,
        }
    }

    pub fn options() -> Vec<&'static str> {
        vec![
            "alexa",
            "americano",
            "blueberry",
            "bumblebee",
            "computer",
            "grapefruit",
            "grasshopper",
            "hey google",
            "hey siri",
            "jarvis",
            "ok google",
            "picovoice",
            "porcupine",
            "terminator",
        ]
    }
}

#[repr(C)]
struct CPorcupine {}

#[repr(C)]
#[derive(PartialEq, Clone, Debug)]
#[allow(non_camel_case_types)]
pub enum PvStatus {
    SUCCESS = 0,
    OUT_OF_MEMORY = 1,
    IO_ERROR = 2,
    INVALID_ARGUMENT = 3,
    STOP_ITERATION = 4,
    KEY_ERROR = 5,
    INVALID_STATE = 6,
    RUNTIME_ERROR = 7,
    ACTIVATION_ERROR = 8,
    ACTIVATION_LIMIT_REACHED = 9,
    ACTIVATION_THROTTLED = 10,
    ACTIVATION_REFUSED = 11,
}

type PvPorcupineInitFn = unsafe extern "C" fn(
    access_key: *const c_char,
    model_path: *const c_char,
    num_keywords: i32,
    keyword_paths: *const *const c_char,
    sensitivities: *const c_float,
    object: *mut *mut CPorcupine,
) -> PvStatus;
type PvSampleRateFn = unsafe extern "C" fn() -> i32;
type PvPorcupineFrameLengthFn = unsafe extern "C" fn() -> i32;
type PvPorcupineVersionFn = unsafe extern "C" fn() -> *mut c_char;
type PvPorcupineProcessFn = unsafe extern "C" fn(
    object: *mut CPorcupine,
    pcm: *const i16,
    keyword_index: *mut i32,
) -> PvStatus;
type PvPorcupineDeleteFn = unsafe extern "C" fn(object: *mut CPorcupine);

#[derive(Clone, Debug)]
pub enum PorcupineErrorStatus {
    LibraryError(PvStatus),
    LibraryLoadError,
    FrameLengthError,
    ArgumentError,
}

#[derive(Clone, Debug)]
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

pub struct PorcupineBuilder {
    access_key: String,
    library_path: PathBuf,
    model_path: PathBuf,
    keyword_paths: Vec<PathBuf>,
    sensitivities: Vec<f32>,
}

impl PorcupineBuilder {
    pub fn new_with_keywords<S: Into<String>>(access_key: S, keywords: &[BuiltinKeywords]) -> Self {
        let default_keyword_paths = pv_keyword_paths();
        let keyword_paths: Vec<String> = keywords
            .iter()
            .map(|keyword| {
                default_keyword_paths
                    .get(keyword.to_str())
                    .expect("Unable to find keyword file for specified keyword")
                    .clone()
            })
            .collect();

        return Self::new_with_keyword_paths(access_key, &keyword_paths);
    }

    pub fn new_with_keyword_paths<S: Into<String>, P: Into<PathBuf> + AsRef<OsStr>>(
        access_key: S,
        keyword_paths: &[P],
    ) -> Self {
        let keyword_paths: Vec<PathBuf> = keyword_paths.iter().map(|path| path.into()).collect();

        let mut sensitivities = Vec::new();
        sensitivities.resize_with(keyword_paths.len(), || 0.5);

        return Self {
            access_key: access_key.into(),
            library_path: pv_library_path(),
            model_path: pv_model_path(),
            keyword_paths,
            sensitivities,
        };
    }

    pub fn access_key<'a, S: Into<String>>(&'a mut self, access_key: S) -> &'a mut Self {
        self.access_key = access_key.into();
        return self;
    }

    pub fn library_path<'a, P: Into<PathBuf>>(&'a mut self, library_path: P) -> &'a mut Self {
        self.library_path = library_path.into();
        return self;
    }

    pub fn model_path<'a, P: Into<PathBuf>>(&'a mut self, model_path: P) -> &'a mut Self {
        self.model_path = model_path.into();
        return self;
    }

    pub fn keyword_paths<'a, P: Into<PathBuf> + AsRef<OsStr>>(
        &'a mut self,
        keyword_paths: &[P],
    ) -> &'a mut Self {
        self.keyword_paths = keyword_paths.iter().map(|path| path.into()).collect();
        return self;
    }

    pub fn sensitivities<'a>(&'a mut self, sensitivities: &[f32]) -> &'a mut Self {
        self.sensitivities = sensitivities.iter().map(|s| *s).collect();
        return self;
    }

    pub fn init(&self) -> Result<Porcupine, PorcupineError> {
        let inner = PorcupineInner::init(
            self.access_key.clone(),
            self.library_path.clone(),
            self.model_path.clone(),
            &self.keyword_paths,
            &self.sensitivities,
        );
        return match inner {
            Ok(inner) => Ok(Porcupine {
                inner: Arc::new(inner),
            }),
            Err(err) => Err(err),
        };
    }
}

#[derive(Clone)]
pub struct Porcupine {
    inner: Arc<PorcupineInner>,
}

impl Porcupine {
    pub fn process(&self, pcm: &[i16]) -> Result<i32, PorcupineError> {
        return self.inner.process(pcm);
    }

    pub fn frame_length(&self) -> u32 {
        return self.inner.frame_length as u32;
    }

    pub fn sample_rate(&self) -> u32 {
        return self.inner.sample_rate as u32;
    }

    pub fn version(&self) -> String {
        return self.inner.version.clone();
    }
}

fn load_library_fn<T>(function_name: &[u8]) -> Result<Symbol<T>, PorcupineError> {
    match &*PV_PORCUPINE_LIB {
        Ok(lib) => unsafe {
            lib.get(function_name).map_err(|err| {
                PorcupineError::new(
                    PorcupineErrorStatus::LibraryLoadError,
                    &format!(
                        "Failed to load function symbol from porcupine library: {}",
                        err
                    ),
                )
            })
        },
        Err(err) => Err((*err).clone()),
    }
}

macro_rules! check_fn_call_status {
    ($status:ident, $function_name:literal) => {
        if $status != PvStatus::SUCCESS {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::LibraryError($status),
                &format!(
                    "Function '{}' in the porcupine library failed",
                    $function_name
                ),
            ));
        }
    };
}

struct PorcupineInnerVTable {
    pv_porcupine_process: Symbol<'static, PvPorcupineProcessFn>,
    pv_porcupine_delete: Symbol<'static, PvPorcupineDeleteFn>,
}

struct PorcupineInner {
    cporcupine: *mut CPorcupine,
    frame_length: i32,
    sample_rate: i32,
    version: String,
    vtable: PorcupineInnerVTable,
}

impl PorcupineInner {
    pub fn init<S: Into<String>, P: Into<PathBuf> + AsRef<OsStr>>(
        access_key: S,
        library_path: P,
        model_path: P,
        keyword_paths: &[P],
        sensitivities: &[f32],
    ) -> Result<Self, PorcupineError> {
        unsafe {
            let library_path: PathBuf = library_path.into();
            let model_path: PathBuf = model_path.into();
            let keyword_paths: Vec<PathBuf> =
                keyword_paths.iter().map(|path| path.into()).collect();

            if !library_path.exists() {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    &format!(
                        "Couldn't find Porcupine's dynamic library at {}",
                        library_path.display()
                    ),
                ));
            }

            if !model_path.exists() {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    &format!("Couldn't find model file at {}", model_path.display()),
                ));
            }

            if keyword_paths.len() == 0 {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    "Keywords should be length of at least one",
                ));
            }

            if sensitivities.len() == 0 {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    "Sensitivities should be length of at least one",
                ));
            }

            if keyword_paths.len() != sensitivities.len() {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    &format!(
                        "Number of keywords ({}) does not match the number of sensitivities ({})",
                        keyword_paths.len(),
                        sensitivities.len()
                    ),
                ));
            }

            for keyword_path in &keyword_paths {
                if !keyword_path.exists() {
                    return Err(PorcupineError::new(
                        PorcupineErrorStatus::ArgumentError,
                        &format!("Couldn't find keyword file at {}", keyword_path.display()),
                    ));
                }
            }

            for sensitivity in sensitivities {
                if *sensitivity < 0.0 || *sensitivity > 1.0 {
                    return Err(PorcupineError::new(
                        PorcupineErrorStatus::ArgumentError,
                        &format!("Sensitivity value {} should be within [0, 1]", sensitivity),
                    ));
                }
            }

            let pv_porcupine_init: Symbol<PvPorcupineInitFn> =
                load_library_fn(b"pv_porcupine_init")?;

            let access_key = match CString::new(access_key.into()) {
                Ok(access_key) => access_key,
                Err(err) => {
                    return Err(PorcupineError::new(
                        PorcupineErrorStatus::ArgumentError,
                        &format!("AccessKey is not a valid C string {}", err),
                    ))
                }
            };
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
                access_key.as_ptr(),
                pv_model_path.as_ptr(),
                pv_keyword_paths.len() as i32,
                pv_keyword_paths_ptrs.as_ptr(),
                sensitivities.as_ptr(),
                addr_of_mut!(cporcupine),
            );
            if status != PvStatus::SUCCESS {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::LibraryError(status),
                    "Failed to initialize the Porcupine library",
                ));
            }

            let pv_porcupine_process: Symbol<PvPorcupineProcessFn> =
                load_library_fn(b"pv_porcupine_process")?;

            let pv_porcupine_delete: Symbol<PvPorcupineDeleteFn> =
                load_library_fn(b"pv_porcupine_delete")?;

            let pv_sample_rate: Symbol<PvSampleRateFn> = load_library_fn(b"pv_sample_rate")?;

            let pv_porcupine_frame_length: Symbol<PvPorcupineFrameLengthFn> =
                load_library_fn(b"pv_porcupine_frame_length")?;

            let pv_porcupine_version: Symbol<PvPorcupineVersionFn> =
                load_library_fn(b"pv_porcupine_version")?;

            let sample_rate = pv_sample_rate();
            let frame_length = pv_porcupine_frame_length();
            let version = match CStr::from_ptr(pv_porcupine_version()).to_str() {
                Ok(string) => string.to_string(),
                Err(err) => {
                    return Err(PorcupineError::new(
                        PorcupineErrorStatus::LibraryLoadError,
                        &format!("Failed to get version info from Porcupine Library: {}", err),
                    ))
                }
            };

            let vtable = PorcupineInnerVTable {
                pv_porcupine_process,
                pv_porcupine_delete,
            };

            return Ok(Self {
                cporcupine,
                sample_rate,
                frame_length,
                version,
                vtable,
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
            (self.vtable.pv_porcupine_process)(self.cporcupine, pcm.as_ptr(), addr_of_mut!(result))
        };
        check_fn_call_status!(status, "pv_porcupine_process");

        return Ok(result);
    }
}

unsafe impl Send for PorcupineInner {}
unsafe impl Sync for PorcupineInner {}

impl Drop for PorcupineInner {
    fn drop(&mut self) {
        unsafe {
            (self.vtable.pv_porcupine_delete)(self.cporcupine);
        }
    }
}
