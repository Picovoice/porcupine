import fnmatch
import os
import re
import subprocess
from argparse import ArgumentParser

import sys

IGNORE_LIST = {
    'demo/c/dr_libs',
    'demo/c/pvrecorder',
    'lib'
}

EXCLUDE_PATTERN = {
    '.*ios.*',
    '.*node_modules.*',
    '.*?/mcu(?!.*src/(pv|main)).*',
    '.*build.*',
}


def main():
    parser = ArgumentParser()

    parser.add_argument('--verbose',
                        '-v',
                        action='store_true',
                        help='If set, shows the list of processed files')
    parser.add_argument('--check-only',
                        '-c',
                        action='store_true',
                        help='If set, checks for warnings only')

    input_args = parser.parse_args()
    formatter(input_args.verbose, input_args.check_only)


def formatter(verbose, check_only):
    if check_only:
        cmd = "clang-format --dry-run --Werror --verbose"
    else:
        cmd = "clang-format -i -style=file --verbose"
    print(cmd)

    src_dir = os.path.join(os.path.abspath(os.path.dirname(__file__)), '../../../')
    all_files = find('*.c', src_dir)
    all_files.extend(find('*.h', src_dir))

    c_source_files = [file_path for file_path in all_files if
                      not any(ignored_path in file_path for ignored_path in IGNORE_LIST)]

    c_source_files = [file_path for file_path in c_source_files if
                      not any(re.match(pattern, file_path, flags=re.IGNORECASE) for pattern in EXCLUDE_PATTERN)]

    c_source_files_num = len(c_source_files)
    for index, c_source_file in enumerate(c_source_files):
        format_command = f"{cmd} {c_source_file}"
        try:
            result = subprocess.check_output(format_command, shell=True, stderr=subprocess.STDOUT).decode('utf-8')
        except subprocess.CalledProcessError as e:
            print(f'Formatter failed with ({e.returncode}):\n{e.output.decode("utf-8")}')
            continue

        if verbose:
            print(result)
        sys.stdout.write(f"Completion: {index / c_source_files_num * 100:.2f}%\r")
        sys.stdout.flush()


def find(pattern, path):
    file_list = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                file_list.append(os.path.join(root, name))
    return file_list


if __name__ == '__main__':
    main()
