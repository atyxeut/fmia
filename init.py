# SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
# SPDX-License-Identifier: GPL-3.0-or-later

import argparse
import subprocess

# > init.py llvm
#   specify `llvm` toolchain and build in debug mode (default)
# > init.py llvm release
#   specify `llvm` toolchain and build in release mode
# > init.py gcc
#   specify `gcc` toolchain and build in debug mode (default)
# > init.py gcc release
#   specify `gcc` toolchain and build in release mode
parser = argparse.ArgumentParser(add_help=False)
parser.add_argument("toolchain", type=str)
parser.add_argument("build_mode", type=str, nargs="?", default="debug")
argv = parser.parse_args()


def main():
  subprocess.run(["python3", "clean.py"], check=False)
  subprocess.run(["xmake", "f", "-v", "--toolchain=" + argv.toolchain, "-m", argv.build_mode], check=False)
  subprocess.run(["xmake", "project", "-k", "compile_commands", "--outputdir=build"], check=False, text=True)


if __name__ == "__main__":
  main()
