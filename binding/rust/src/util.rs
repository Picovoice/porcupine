/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use std::collections::HashMap;
use std::ffi::CString;
use std::fs;
use std::path::{Path, PathBuf};

const DEFAULT_RELATIVE_KEYWORDS_DIR: &str = "resources/keyword_files/";
const DEFAULT_RELATIVE_LIBRARY_DIR: &str = "lib/";
const DEFAULT_RELATIVE_MODEL_PATH: &str = "lib/common/porcupine_params.pv";

#[allow(dead_code)]
const RPI_MACHINES: [&str; 4] = ["arm11", "cortex-a7", "cortex-a53", "cortex-a72"];
#[allow(dead_code)]
const JETSON_MACHINES: [&str; 1] = ["cortex-a57"];

#[cfg(all(target_os = "linux", any(target_arch = "arm", target_arch = "aarch64")))]
fn find_machine_type() -> String {
    use std::process::Command;

    let cpu_info = Command::new("cat")
        .arg("/proc/cpuinfo")
        .output()
        .expect("Failed to retrieve cpu info");
    let cpu_part_list = std::str::from_utf8(&cpu_info.stdout)
        .unwrap()
        .split("\n")
        .filter(|x| x.contains("CPU part"))
        .collect::<Vec<_>>();

    if cpu_part_list.len() == 0 {
        panic!("Unsupported CPU");
    }

    let cpu_part = cpu_part_list[0]
        .split(" ")
        .collect::<Vec<_>>()
        .pop()
        .unwrap()
        .to_lowercase();

    let machine = match cpu_part.as_str() {
        "0xb76" => "arm11",
        "0xc07" => "cortex-a7",
        "0xd03" => "cortex-a53",
        "0xd07" => "cortex-a57",
        "0xd08" => "cortex-a72",
        "0xc08" => "beaglebone",
        _ => "unsupported",
    };

    String::from(machine)
}

#[cfg(all(target_os = "macos", target_arch = "x86_64"))]
fn base_library_path() -> PathBuf {
    PathBuf::from("mac/x86_64/libpv_porcupine.dylib")
}

#[cfg(all(target_os = "macos", target_arch = "aarch64"))]
fn base_library_path() -> PathBuf {
    PathBuf::from("mac/arm64/libpv_porcupine.dylib")
}

#[cfg(target_os = "windows")]
fn base_library_path() -> PathBuf {
    PathBuf::from("windows/amd64/libpv_porcupine.dll")
}

#[cfg(all(target_os = "linux", target_arch = "x86_64"))]
fn base_library_path() -> PathBuf {
    PathBuf::from("linux/x86_64/libpv_porcupine.so")
}

#[cfg(all(target_os = "linux", any(target_arch = "arm", target_arch = "aarch64")))]
fn base_library_path() -> PathBuf {
    let machine = find_machine_type();
    match machine.as_str() {
        machine if RPI_MACHINES.contains(&machine) => {
            if cfg!(target_arch = "aarch64") {
                PathBuf::from(format!(
                    "raspberry-pi/{}-aarch64/libpv_porcupine.so",
                    &machine
                ))
            } else {
                PathBuf::from(format!("raspberry-pi/{}/libpv_porcupine.so", &machine))
            }
        }
        machine if JETSON_MACHINES.contains(&machine) => {
            PathBuf::from("jetson/cortex-a57-aarch64/libpv_porcupine.so")
        }
        "beaglebone" => PathBuf::from("beaglebone/libpv_porcupine.so"),
        _ => {
            eprintln!("WARNING: Please be advised that this device is not officially supported by Picovoice.\nFalling back to the armv6-based (Raspberry Pi Zero) library. This is not tested nor optimal.\nFor the model, use Raspberry Pi's models");
            PathBuf::from("raspberry-pi/arm11/libpv_porcupine.so")
        }
    }
}

pub fn pv_library_path() -> PathBuf {
    let mut path = PathBuf::from(env!("OUT_DIR"));
    path.push(DEFAULT_RELATIVE_LIBRARY_DIR);
    path.push(base_library_path());
    path
}

pub fn pv_model_path() -> PathBuf {
    let mut path = PathBuf::from(env!("OUT_DIR"));
    path.push(DEFAULT_RELATIVE_MODEL_PATH);
    path
}

#[cfg(target_os = "macos")]
pub fn pv_platform() -> String {
    String::from("mac")
}

#[cfg(target_os = "windows")]
pub fn pv_platform() -> String {
    String::from("windows")
}

#[cfg(all(target_os = "linux", target_arch = "x86_64"))]
pub fn pv_platform() -> String {
    String::from("linux")
}

#[cfg(all(target_os = "linux", any(target_arch = "arm", target_arch = "aarch64")))]
pub fn pv_platform() -> String {
    let machine = find_machine_type();
    match machine.as_str() {
        machine if RPI_MACHINES.contains(&machine) => String::from("raspberry-pi"),
        machine if JETSON_MACHINES.contains(&machine) => String::from("jetson"),
        "beaglebone" => String::from("beaglebone"),
        _ => {
            panic!("ERROR: Please be advised that this device is not officially supported by Picovoice");
        }
    }
}

pub fn pv_keyword_paths() -> HashMap<String, String> {
    let pv_platform = pv_platform();
    let keyword_file_pattern = format!("_{pv_platform}.ppn");

    let mut dir = PathBuf::from(env!("OUT_DIR"));
    dir.push(DEFAULT_RELATIVE_KEYWORDS_DIR);
    dir.push(pv_platform);

    let mut keyword_paths = HashMap::new();
    let dir_entries = fs::read_dir(&dir)
        .unwrap_or_else(|_| panic!("Can't find default keyword_files dir: {}", dir.display()));

    for entry in dir_entries.flatten() {
        let path = entry.path();
        let keyword_string = entry.file_name().into_string().unwrap();

        if keyword_string.contains(&keyword_file_pattern)
            && keyword_string.len() > keyword_file_pattern.len()
        {
            if let Some(keyword) = keyword_string.split('_').next() {
                keyword_paths.insert(
                    keyword.to_string(),
                    path.into_os_string().into_string().unwrap(),
                );
            }
        }
    }

    keyword_paths
}

pub fn pathbuf_to_cstring<P: AsRef<Path>>(pathbuf: P) -> CString {
    let pathstr = pathbuf.as_ref().to_str().unwrap();
    CString::new(pathstr).unwrap()
}
