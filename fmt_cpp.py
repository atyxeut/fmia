# SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
# SPDX-License-Identifier: GPL-3.0-or-later

import argparse
import subprocess
import sys
from pathlib import Path

# > fmt_cpp.py path
#                       ↑ this is required
#   1. if path is a folder, recursively format every subpath in it
#   2. if path is a .hpp/.cppm/.cpp/ file, format it using clang-format, styles are defined in .clang-format
parser = argparse.ArgumentParser(add_help=False)
parser.add_argument("path", nargs="*", type=str)
argv = parser.parse_args()


def fmt_cpp(path: Path):
  if path.is_dir():
    for item in path.iterdir():
      fmt_cpp(item)
  elif path.suffix == ".hpp" or path.suffix == ".cpp" or path.suffix == ".cppm":
    print(f"formatting {path}")
    subprocess.run(["clang-format", "-style=file:.clang-format", "-i", path], check=False)


def main():
  for path in argv.path:
    fmt_cpp(Path(path).resolve())
  print("format done", file=sys.stderr)


if __name__ == "__main__":
  main()
