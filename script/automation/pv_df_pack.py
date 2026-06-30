import base64
import argparse
import os


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', required=True)

    parser.add_argument('--unpack', action='store_true')

    args = parser.parse_args()

    found_files = list()
    for (root, dirs, files) in os.walk(args.root):
        if 'test_resources' in root:
            for name in files:
                if ".ppn" in name or ".wav" in name or ".pv" in name:
                    found_files.append(os.path.join(root, name))

    for file in found_files:
        filename, extension = os.path.basename(file).split(".")
        new_filename = base64.urlsafe_b64encode(filename.encode('utf8')) if not args.unpack else base64.urlsafe_b64decode(filename.encode('utf8'))
        new_filename = new_filename.decode('utf8')

        old_path = file
        new_path = os.path.join(os.path.dirname(file), f"{new_filename}.{extension}")
        print(f"Renaming {old_path} to {new_path}")
        os.rename(old_path, new_path)
