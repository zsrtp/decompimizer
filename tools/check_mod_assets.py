#!/usr/bin/env python3

import hashlib
import os
import sys


def compute_assets_hash(assets_dir: str) -> str:
    h = hashlib.md5()
    if not os.path.isdir(assets_dir):
        return h.hexdigest()

    for root, dirs, files in os.walk(assets_dir):
        dirs.sort()
        for fname in sorted(files):
            fpath = os.path.join(root, fname)
            rel = os.path.relpath(fpath, assets_dir)
            h.update(rel.encode())
            with open(fpath, "rb") as f:
                while True:
                    chunk = f.read(65536)
                    if not chunk:
                        break
                    h.update(chunk)
    return h.hexdigest()


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <stamp_file>", file=sys.stderr)
        sys.exit(1)

    stamp_file = sys.argv[1]
    new_hash = compute_assets_hash("mod_assets")

    old_hash = ""
    if os.path.isfile(stamp_file):
        with open(stamp_file, "r") as f:
            old_hash = f.read().strip()

    if new_hash != old_hash:
        with open(stamp_file, "w") as f:
            f.write(new_hash + "\n")


if __name__ == "__main__":
    main()
