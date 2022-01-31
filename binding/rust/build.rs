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

fn copy_dir<U: AsRef<Path>, V: AsRef<Path>>(from: U, to: V) -> Result<(), std::io::Error> {
    let mut stack = vec![PathBuf::from(from.as_ref())];

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
            } else if let Some(filename) = path.file_name() {
                let dest_path = dest.join(filename);
                fs::copy(&path, &dest_path)?;
            }
        }
    }

    Ok(())
}

fn main() {
    let base_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap()).join("data/");
    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());

    let lib_base_dir = base_dir.join("lib/");
    let lib_out_dir = out_dir.join("lib/");
    copy_dir(lib_base_dir, lib_out_dir).unwrap();

    let resources_base_dir = base_dir.join("resources/");
    let resources_out_dir = out_dir.join("resources/");
    copy_dir(resources_base_dir, resources_out_dir).unwrap();
}
