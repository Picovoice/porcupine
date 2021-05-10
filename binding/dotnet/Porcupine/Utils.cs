/*
    Copyright 2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

using System.Collections.Generic;
using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Pv
{
    public static class Utils
    {
        private static string _env => RuntimeInformation.IsOSPlatform(OSPlatform.OSX) ? "mac" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Linux) ? "linux" :
                                                 RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "windows" : "";

        private static Architecture _arch => RuntimeInformation.ProcessArchitecture;
        public static string PvModelPath()
        {
            return Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "lib/common/porcupine_params.pv");
        }

        public static Dictionary<string, string> PvKeywordPaths()
        {
            string keywordFilesDir = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "resources/keyword_files", _env);

            Dictionary<string, string> keywordPaths = new Dictionary<string, string>();
            foreach (string keywordFile in Directory.GetFiles(keywordFilesDir))
            {
                keywordPaths.Add(Path.GetFileName(keywordFile).Split('_')[0], Path.Combine(keywordFilesDir, keywordFile));
            }

            return keywordPaths;
        }

        public static string PvLibraryPath(string libName)
        {            
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return $"./lib/windows/amd64/{libName}.dll";
            }
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                return $"./lib/mac/x86_64/{libName}.dylib";                
            }
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                string path = $"./lib/{PvLinuxMachine()}/{libName}.so";
                Console.WriteLine(path);
                return path;
            }
            else
            {
                throw new PlatformNotSupportedException($"Unsupported Platform.\n");
            }
        }

        public static string PvLinuxMachine() 
        {
            string archInfo = "";
            if (_arch == Architecture.X64)
                return Path.Combine("linux", "x86_64");
            else if (_arch == Architecture.Arm64)
                archInfo = "-aarch64";

            string cpuInfo = File.ReadAllText("/proc/cpuinfo");
            string[] cpuPartList = cpuInfo.Split('\n').Where(x => x.Contains("CPU part")).ToArray();
            if (cpuPartList.Length == 0)
                throw new PlatformNotSupportedException($"Unsupported CPU.\n{cpuInfo}");

            string cpuPart = cpuPartList[0].Split(" ").Last().ToLower();
            switch (cpuPart)
            {
                case "0xb76": return Path.Combine("raspberry-pi", "arm11" + archInfo);
                case "0xc07": return Path.Combine("raspberry-pi", "cortex-a7" + archInfo);
                case "0xd03": return Path.Combine("raspberry-pi", "cortex-a53" + archInfo);
                case "0xd07": return Path.Combine("raspberry-pi", "cortex-a57" + archInfo);
                case "0xd08": return Path.Combine("raspberry-pi", "cortex-a72" + archInfo);
                default:
                    Console.WriteLine(
                        $"WARNING: Please be advised that this device (CPU part = {cpuPart}) is not officially supported by Picovoice. " +
                        "Falling back to the armv6-based (Raspberry Pi Zero) library. This is not tested nor optimal.\n For the model, use Raspberry Pi\'s models");
                    return Path.Combine("raspberry-pi", "arm11" + archInfo);
            }
        }
    }
}
