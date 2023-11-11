/*
    Copyright 2021-2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use std::cmp::PartialEq;
use std::ffi::OsStr;
use std::ffi::{CStr, CString};
use std::path::{Path, PathBuf};
use std::ptr::addr_of_mut;
use std::str::FromStr;
use std::sync::Arc;

use libc::{c_char, c_float};
use libloading::{Library, Symbol};

use crate::util::{pathbuf_to_cstring, pv_keyword_paths, pv_library_path, pv_model_path};

#[cfg(unix)]
use libloading::os::unix::Symbol as RawSymbol;
#[cfg(windows)]
use libloading::os::windows::Symbol as RawSymbol;

#[derive(PartialEq, Eq, Clone, Debug)]
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

impl FromStr for BuiltinKeywords {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s {
            "alexa" => Ok(Self::Alexa),
            "americano" => Ok(Self::Americano),
            "blueberry" => Ok(Self::Blueberry),
            "bumblebee" => Ok(Self::Bumblebee),
            "computer" => Ok(Self::Computer),
            "grapefruit" => Ok(Self::Grapefruit),
            "grasshopper" => Ok(Self::Grasshopper),
            "hey google" => Ok(Self::HeyGoogle),
            "hey siri" => Ok(Self::HeySiri),
            "jarvis" => Ok(Self::Jarvis),
            "ok google" => Ok(Self::OkGoogle),
            "picovoice" => Ok(Self::Picovoice),
            "porcupine" => Ok(Self::Porcupine),
            "terminator" => Ok(Self::Terminator),
            _ => Err(()),
        }
    }
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
struct CPorcupine {
    // Fields suggested by the Rustonomicon: https://doc.rust-lang.org/nomicon/ffi.html#representing-opaque-structs
    _data: [u8; 0],
    _marker: core::marker::PhantomData<(*mut u8, core::marker::PhantomPinned)>,
}

#[repr(C)]
#[derive(PartialEq, Eq, Clone, Debug)]
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
type PvPorcupineDeleteFn = unsafe extern "C" fn(object: *mut CPorcupine);
type PvPorcupineProcessFn = unsafe extern "C" fn(
    object: *mut CPorcupine,
    pcm: *const i16,
    keyword_index: *mut i32,
) -> PvStatus;
type PvSampleRateFn = unsafe extern "C" fn() -> i32;
type PvPorcupineFrameLengthFn = unsafe extern "C" fn() -> i32;
type PvPorcupineVersionFn = unsafe extern "C" fn() -> *mut c_char;
type PvGetErrorStackFn =
    unsafe extern "C" fn(message_stack: *mut *mut *mut c_char, message_stack_depth: *mut i32) -> PvStatus;
type PvFreeErrorStackFn = unsafe extern "C" fn(message_stack: *mut *mut c_char);
type PvSetSdkFn = unsafe extern "C" fn(sdk: *const c_char);

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
    pub message: String,
    pub message_stack: Vec<String>,
}

impl PorcupineError {
    pub fn new(status: PorcupineErrorStatus, message: impl Into<String>) -> Self {
        Self {
            status,
            message: message.into(),
            message_stack: Vec::new()
        }
    }

    pub fn new_with_stack(
        status: PorcupineErrorStatus,
        message: impl Into<String>,
        message_stack: impl Into<Vec<String>>
    ) -> Self {
        Self {
            status,
            message: message.into(),
            message_stack: message_stack.into(),
        }
    }
}

impl std::fmt::Display for PorcupineError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> Result<(), std::fmt::Error> {
        let mut message_string = String::new();
        message_string.push_str(&format!("{} with status '{:?}'", self.message, self.status));

        if !self.message_stack.is_empty() {
            message_string.push(':');
            for x in 0..self.message_stack.len() {
                message_string.push_str(&format!("  [{}] {}\n", x, self.message_stack[x]))
            };
        }
        write!(f, "{}", message_string)
    }
}

impl std::error::Error for PorcupineError {}

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
        let keyword_paths = keywords
            .iter()
            .map(|keyword| {
                default_keyword_paths
                    .get(keyword.to_str())
                    .expect("Unable to find keyword file for specified keyword")
            })
            .cloned()
            .collect::<Vec<_>>();

        Self::new_with_keyword_paths(access_key, &keyword_paths)
    }

    pub fn new_with_keyword_paths<S: Into<String>, P: Into<PathBuf> + AsRef<OsStr>>(
        access_key: S,
        keyword_paths: &[P],
    ) -> Self {
        let keyword_paths = keyword_paths
            .iter()
            .map(|path| path.into())
            .collect::<Vec<_>>();

        let mut sensitivities = Vec::new();
        sensitivities.resize_with(keyword_paths.len(), || 0.5);

        Self {
            access_key: access_key.into(),
            library_path: pv_library_path(),
            model_path: pv_model_path(),
            keyword_paths,
            sensitivities,
        }
    }

    pub fn access_key<S: Into<String>>(&mut self, access_key: S) -> &mut Self {
        self.access_key = access_key.into();
        self
    }

    pub fn library_path<P: Into<PathBuf>>(&mut self, library_path: P) -> &mut Self {
        self.library_path = library_path.into();
        self
    }

    pub fn model_path<P: Into<PathBuf>>(&mut self, model_path: P) -> &mut Self {
        self.model_path = model_path.into();
        self
    }

    pub fn keyword_paths<P: Into<PathBuf> + AsRef<OsStr>>(
        &mut self,
        keyword_paths: &[P],
    ) -> &mut Self {
        self.keyword_paths = keyword_paths.iter().map(|path| path.into()).collect();
        self
    }

    pub fn sensitivities(&mut self, sensitivities: &[f32]) -> &mut Self {
        self.sensitivities = sensitivities.to_vec();
        self
    }

    pub fn init(&self) -> Result<Porcupine, PorcupineError> {
        let inner = PorcupineInner::init(
            &self.access_key,
            &self.library_path,
            &self.model_path,
            &self.keyword_paths,
            &self.sensitivities,
        );
        match inner {
            Ok(inner) => Ok(Porcupine {
                inner: Arc::new(inner),
            }),
            Err(err) => Err(err),
        }
    }
}

#[derive(Clone)]
pub struct Porcupine {
    inner: Arc<PorcupineInner>,
}

impl Porcupine {
    pub fn process(&self, pcm: &[i16]) -> Result<i32, PorcupineError> {
        self.inner.process(pcm)
    }

    pub fn frame_length(&self) -> u32 {
        self.inner.frame_length as u32
    }

    pub fn sample_rate(&self) -> u32 {
        self.inner.sample_rate as u32
    }

    pub fn version(&self) -> &str {
        &self.inner.version
    }
}

unsafe fn load_library_fn<T>(
    library: &Library,
    function_name: &[u8],
) -> Result<RawSymbol<T>, PorcupineError> {
    library
        .get(function_name)
        .map(|s: Symbol<T>| s.into_raw())
        .map_err(|err| {
            PorcupineError::new(
                PorcupineErrorStatus::LibraryLoadError,
                format!("Failed to load function symbol from porcupine library: {err}"),
            )
        })
}

fn check_fn_call_status(
    vtable: &PorcupineInnerVTable,
    status: PvStatus,
    function_name: &str,
) -> Result<(), PorcupineError> {
    match status {
        PvStatus::SUCCESS => Ok(()),
        _ => unsafe {
            let mut message_stack_ptr: *mut c_char = std::ptr::null_mut();
            let mut message_stack_ptr_ptr = addr_of_mut!(message_stack_ptr);

            let mut message_stack_depth: i32 = 0;
            let err_status = (vtable.pv_get_error_stack)(
                addr_of_mut!(message_stack_ptr_ptr),
                addr_of_mut!(message_stack_depth),
            );

            if err_status != PvStatus::SUCCESS {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::LibraryError(err_status),
                    "Unable to get Porcupine error state",
                ));
            };

            let mut message_stack = Vec::new();
            for i in 0..message_stack_depth as usize {
                let message = CStr::from_ptr(*message_stack_ptr_ptr.add(i));
                let message = message.to_string_lossy().into_owned();
                message_stack.push(message);
            }

            (vtable.pv_free_error_stack)(message_stack_ptr_ptr);

            Err(PorcupineError::new_with_stack(
                PorcupineErrorStatus::LibraryError(status),
                format!("'{function_name}' failed"),
                message_stack,
            ))
        },
    }
}

struct PorcupineInnerVTable {
    pv_porcupine_init: RawSymbol<PvPorcupineInitFn>,
    pv_porcupine_delete: RawSymbol<PvPorcupineDeleteFn>,
    pv_porcupine_process: RawSymbol<PvPorcupineProcessFn>,
    pv_sample_rate: RawSymbol<PvSampleRateFn>,
    pv_porcupine_frame_length: RawSymbol<PvPorcupineFrameLengthFn>,
    pv_porcupine_version: RawSymbol<PvPorcupineVersionFn>,
    pv_get_error_stack: RawSymbol<PvGetErrorStackFn>,
    pv_free_error_stack: RawSymbol<PvFreeErrorStackFn>,
    pv_set_sdk: RawSymbol<PvSetSdkFn>,

    _lib_guard: Library,
}

impl PorcupineInnerVTable {
    pub fn new(lib: Library) -> Result<Self, PorcupineError> {
        // SAFETY: the library will be hold by this struct and therefore the symbols can't outlive the library
        unsafe {
            Ok(Self {
                pv_porcupine_init: load_library_fn(&lib, b"pv_porcupine_init")?,
                pv_porcupine_delete: load_library_fn(&lib, b"pv_porcupine_delete")?,
                pv_porcupine_process: load_library_fn(&lib, b"pv_porcupine_process")?,
                pv_sample_rate: load_library_fn(&lib, b"pv_sample_rate")?,
                pv_porcupine_frame_length: load_library_fn(&lib, b"pv_porcupine_frame_length")?,
                pv_porcupine_version: load_library_fn(&lib, b"pv_porcupine_version")?,
                pv_get_error_stack: load_library_fn(&lib, b"pv_get_error_stack")?,
                pv_free_error_stack: load_library_fn(&lib, b"pv_free_error_stack")?,
                pv_set_sdk: load_library_fn(&lib, b"pv_set_sdk")?,

                _lib_guard: lib,
            })
        }
    }
}

struct PorcupineInner {
    cporcupine: *mut CPorcupine,
    frame_length: i32,
    sample_rate: i32,
    version: String,
    vtable: PorcupineInnerVTable,
}

impl PorcupineInner {
    pub fn init<P: AsRef<Path>>(
        access_key: &str,
        library_path: P,
        model_path: P,
        keyword_paths: &[PathBuf],
        sensitivities: &[f32],
    ) -> Result<Self, PorcupineError> {
        if !library_path.as_ref().exists() {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::ArgumentError,
                format!(
                    "Couldn't find Porcupine's dynamic library at {}",
                    library_path.as_ref().display()
                ),
            ));
        }

        if !model_path.as_ref().exists() {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::ArgumentError,
                format!(
                    "Couldn't find model file at {}",
                    model_path.as_ref().display()
                ),
            ));
        }

        if keyword_paths.is_empty() {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::ArgumentError,
                "Keywords should be length of at least one",
            ));
        }

        if sensitivities.is_empty() {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::ArgumentError,
                "Sensitivities should be length of at least one",
            ));
        }

        if keyword_paths.len() != sensitivities.len() {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::ArgumentError,
                format!(
                    "Number of keywords ({}) does not match the number of sensitivities ({})",
                    keyword_paths.len(),
                    sensitivities.len()
                ),
            ));
        }

        for keyword_path in keyword_paths.iter() {
            if !keyword_path.exists() {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    format!("Couldn't find keyword file at {}", keyword_path.display()),
                ));
            }
        }

        for sensitivity in sensitivities {
            if !(0.0..=1.0).contains(sensitivity) {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    format!("Sensitivity value {sensitivity} should be within [0, 1]"),
                ));
            }
        }

        let lib = unsafe { Library::new(library_path.as_ref()) }.map_err(|err| {
            PorcupineError::new(
                PorcupineErrorStatus::LibraryLoadError,
                format!("Failed to load porcupine dynamic library: {err}"),
            )
        })?;
        let vtable = PorcupineInnerVTable::new(lib)?;

        let sdk_string = match CString::new("rust") {
            Ok(sdk_string) => sdk_string,
            Err(err) => {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    format!("sdk_string is not a valid C string {err}"),
                ))
            }
        };

        let access_key = match CString::new(access_key) {
            Ok(access_key) => access_key,
            Err(err) => {
                return Err(PorcupineError::new(
                    PorcupineErrorStatus::ArgumentError,
                    format!("AccessKey is not a valid C string {err}"),
                ))
            }
        };
        let mut cporcupine = std::ptr::null_mut();
        let pv_model_path = pathbuf_to_cstring(&model_path);
        let pv_keyword_paths = keyword_paths
            .iter()
            .map(pathbuf_to_cstring)
            .collect::<Vec<_>>();
        let pv_keyword_paths_ptrs = pv_keyword_paths
            .iter()
            .map(|keyword_path| keyword_path.as_ptr())
            .collect::<Vec<_>>();

        // SAFETY: most of the unsafe comes from the `load_library_fn` which is
        // safe, because we don't use the raw symbols after this function
        // anymore.
        let (sample_rate, frame_length, version) = unsafe {
            (vtable.pv_set_sdk)(sdk_string.as_ptr());

            let status = (vtable.pv_porcupine_init)(
                access_key.as_ptr(),
                pv_model_path.as_ptr(),
                pv_keyword_paths.len() as i32,
                pv_keyword_paths_ptrs.as_ptr(),
                sensitivities.as_ptr(),
                addr_of_mut!(cporcupine),
            );
            check_fn_call_status(&vtable, status, "pv_porcupine_init")?;

            let version = CStr::from_ptr((vtable.pv_porcupine_version)())
                .to_string_lossy()
                .into_owned();

            (
                (vtable.pv_sample_rate)(),
                (vtable.pv_porcupine_frame_length)(),
                version,
            )
        };

        Ok(Self {
            cporcupine,
            sample_rate,
            frame_length,
            version,
            vtable,
        })
    }

    pub fn process(&self, pcm: &[i16]) -> Result<i32, PorcupineError> {
        if pcm.len() as i32 != self.frame_length {
            return Err(PorcupineError::new(
                PorcupineErrorStatus::FrameLengthError,
                format!(
                    "Found a frame length of {} Expected {}",
                    pcm.len(),
                    self.frame_length
                ),
            ));
        }

        let mut result = -1;
        let status = unsafe {
            (self.vtable.pv_porcupine_process)(self.cporcupine, pcm.as_ptr(), addr_of_mut!(result))
        };
        check_fn_call_status(&self.vtable, status, "pv_porcupine_process")?;

        Ok(result)
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

#[cfg(test)]
mod tests {
    use std::env;
    use std::path::PathBuf;

    use crate::util::{pv_keyword_paths, pv_library_path, pv_model_path};
    use crate::porcupine::{BuiltinKeywords, PorcupineInner};

    #[test]
    fn test_process_error_stack() {
        let access_key = env::var("PV_ACCESS_KEY")
            .expect("Pass the AccessKey in using the PV_ACCESS_KEY env variable");

        let default_keyword_paths = pv_keyword_paths();
        let keyword_path = default_keyword_paths.get(BuiltinKeywords::Porcupine.to_str())
            .expect("Unable to find keyword file for specified keyword");
        
        let mut inner = PorcupineInner::init(
            &access_key.as_str(),
            pv_library_path(),
            pv_model_path(),
            &[PathBuf::from(keyword_path)],
            &[0.5],
        ).expect("Unable to create Porcupine");

        let test_pcm = vec![0; inner.frame_length as usize];
        let address = inner.cporcupine;
        inner.cporcupine = std::ptr::null_mut();

        let res = inner.process(&test_pcm);

        inner.cporcupine = address;
        if let Err(err) = res {
            assert!(err.message_stack.len() > 0);
            assert!(err.message_stack.len() < 8);
        } else {
            assert!(res.unwrap() == 100);
        }
    }
}
