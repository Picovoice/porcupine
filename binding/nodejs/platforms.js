//
// Copyright 2020 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

const fs = require("fs");
const os = require("os");
const path = require("path");

const { PvUnsupportedPlatformError } = require("./errors");

const SYSTEM_LINUX = "linux";
const SYSTEM_MAC = "darwin";

const X86_64 = "x64";
const ARM_32 = "arm";
const ARM_64 = "arm64";

const PLATFORM_MAC = "mac";
const PLATFORM_LINUX = "linux";
const PLATFORM_RASPBERRY_PI = "raspberry-pi";

const ARM_CPU_CORTEX_A7 = "cortex-a7";
const ARM_CPU_CORTEX_A53 = "cortex-a53";
const ARM_CPU_CORTEX_A72 = "cortex-a72";

const SUPPORTED_NODEJS_SYSTEMS = new Set([
  SYSTEM_LINUX,
  SYSTEM_MAC,
]);

const LIBRARY_PATH_PREFIX = "lib/";
const SYSTEM_TO_LIBRARY_PATH = new Map();
SYSTEM_TO_LIBRARY_PATH.set(
  `${SYSTEM_MAC}/${X86_64}`,
  `${PLATFORM_MAC}/x86_64/pv_porcupine.node`
);
SYSTEM_TO_LIBRARY_PATH.set(
  `${SYSTEM_LINUX}/${X86_64}`,
  `${PLATFORM_LINUX}/x86_64/pv_porcupine.node`
);
SYSTEM_TO_LIBRARY_PATH.set(
  `${SYSTEM_LINUX}/${ARM_CPU_CORTEX_A7}`,
  `${PLATFORM_RASPBERRY_PI}/${ARM_CPU_CORTEX_A7}/pv_porcupine.node`
);
SYSTEM_TO_LIBRARY_PATH.set(
  `${SYSTEM_LINUX}/${ARM_CPU_CORTEX_A53}`,
  `${PLATFORM_RASPBERRY_PI}/${ARM_CPU_CORTEX_A53}/pv_porcupine.node`
);
SYSTEM_TO_LIBRARY_PATH.set(
  `${SYSTEM_LINUX}/${ARM_CPU_CORTEX_A72}`,
  `${PLATFORM_RASPBERRY_PI}/${ARM_CPU_CORTEX_A72}/pv_porcupine.node`
);

function absoluteLibraryPath(libraryPath) {
  return path.resolve(__dirname, LIBRARY_PATH_PREFIX, libraryPath);
}

function armLinuxCpuString() {
  const cpuModel = os.cpus()[0].model;
  const cpuInfo = fs.readFileSync("/proc/cpuinfo", "ascii");

  for (let infoLine of cpuInfo.split("\n")) {
    if (infoLine.includes(":")) {
      let infoKeyValue = infoLine.split(":");
      let infoKey = infoKeyValue[0].trim();
      let infoValue = infoKeyValue[1].trim();

      if (infoKey === "Hardware" && infoValue.includes("BCM")) {
        if (cpuModel.includes("rev 5")) {
          return ARM_CPU_CORTEX_A7;
        } else if (cpuModel.includes("rev 4")) {
          return ARM_CPU_CORTEX_A53;
        } else if (cpuModel.includes("rev 3")) {
          return ARM_CPU_CORTEX_A72;
        }
      }
    }
  }
  return null;
}

function getPlatform() {
  const system = os.platform();
  const arch = os.arch();

  if (system === SYSTEM_MAC && arch === X86_64) {
    return PLATFORM_MAC;
  }

  if (system === SYSTEM_LINUX) {
    if (arch === X86_64) {
      return PLATFORM_LINUX;
    } else {
      let armCpu = armLinuxCpuString();

      if (armCpu !== null) {
        return PLATFORM_RASPBERRY_PI;
      }
    }
  }

  throw `System ${system}/${arch} is not supported by this library.`;
}

function getSystemLibraryPath() {
  const system = os.platform();
  const arch = os.arch();

  if (SUPPORTED_NODEJS_SYSTEMS.has(system)) {
    switch (system) {
      case SYSTEM_MAC: {
        if (arch === X86_64) {
          return absoluteLibraryPath(
            SYSTEM_TO_LIBRARY_PATH.get(`${SYSTEM_MAC}/${X86_64}`)
          );
        }
      }
      case SYSTEM_LINUX: {
        if (arch === X86_64) {
          return absoluteLibraryPath(
            SYSTEM_TO_LIBRARY_PATH.get(`${SYSTEM_LINUX}/${X86_64}`)
          );
        } else if (arch === ARM_32 || arch === ARM_64) {
          let armCpu = armLinuxCpuString();
          if (armCpu !== null) {
            return absoluteLibraryPath(
              SYSTEM_TO_LIBRARY_PATH.get(`${SYSTEM_LINUX}/${armCpu}`)
            );
          } else {
            throw new PvUnsupportedPlatformError(
              `System ${system}/${arch} is not supported by this library for this CPU.`
            );
          }
        }
      }
    }
  }

  throw new PvUnsupportedPlatformError(
    `System ${system}/${arch} is not supported by this library.`
  );
}

exports.getPlatform = getPlatform;
exports.getSystemLibraryPath = getSystemLibraryPath;
