/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

use std::env;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

fn copy_dir<U: AsRef<Path>, V: AsRef<Path>>(from: U, to: V) -> Result<(), std::io::Error> {
    let mut stack = Vec::new();
    stack.push(PathBuf::from(from.as_ref()));

    let output_root = PathBuf::from(to.as_ref());
    let input_root = PathBuf::from(from.as_ref()).components().count();

    while let Some(working_path) = stack.pop() {
        let src: PathBuf = working_path.components().skip(input_root).collect();

        let dest = if src.components().count() == 0 {
            output_root.clone()
        } else {
            output_root.join(&src)
        };

        if fs::metadata(&dest).is_err() {
            fs::create_dir_all(&dest)?;
        }

        for entry in fs::read_dir(working_path)? {
            let entry = entry?;
            let path = entry.path();
            if path.is_dir() {
                stack.push(path);
            } else {
                match path.file_name() {
                    Some(filename) => {
                        let dest_path = dest.join(filename);
                        fs::copy(&path, &dest_path)?;
                    }
                    None => {}
                }
            }
        }
    }

    Ok(())
}

fn main() {
    let pwd = Command::new("pwd").output().unwrap();
    let pwd = String::from_utf8_lossy(&pwd.stdout);

    let mut base_dir = PathBuf::from(file!());
    base_dir.pop(); // file! macro includes filename
    if pwd.contains("target") {
        base_dir.push("../../../");
    }
    base_dir.push("../../");

    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());

    let mut lib_base_dir = base_dir.clone();
    let mut lib_out_dir = out_dir.clone();
    lib_base_dir.push("lib/");
    lib_out_dir.push("lib/");
    copy_dir(lib_base_dir, lib_out_dir).unwrap();

    let mut resources_base_dir = base_dir.clone();
    let mut resources_out_dir = out_dir.clone();
    resources_base_dir.push("resources/");
    resources_out_dir.push("resources/");
    copy_dir(resources_base_dir, resources_out_dir).unwrap();

    println!("cargo:rerun-if-changed=build.rs");
}
