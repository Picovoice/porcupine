/*
    Copyright 2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;

namespace PorcupineDotNet
{
    public static class Utils
    {
        private static string _env => RuntimeInformation.IsOSPlatform(OSPlatform.OSX) ? "mac" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Linux) ? "linux" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "windows" : "";
        public static string PvModelPath(string relativeDir)
        {
            return Path.Combine(relativeDir, "lib/common/porcupine_params.pv");
        }

        public static Dictionary<string, string> PvKeywordPaths(string relativeDir)
        {
            string keywordFilesDir = Path.Combine(relativeDir, "resources/keyword_files", _env);

            Dictionary<string, string> keywordPaths = new Dictionary<string, string>();
            foreach (string keywordFile in Directory.GetFiles(keywordFilesDir))
            {
                keywordPaths.Add(Path.GetFileName(keywordFile).Split('_')[0], Path.Combine(keywordFilesDir, keywordFile));
            }

            return keywordPaths;
        }
    }
}
