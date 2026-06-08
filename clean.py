# SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
# SPDX-License-Identifier: GPL-3.0-or-later

import shutil
from pathlib import Path


def remove(path: Path):
  if not path.exists():
    return
  if path.is_dir():
    shutil.rmtree(path)
  else:
    path.unlink()


def main():
  remove(Path(".cache"))
  remove(Path(".xmake"))
  remove(Path("build"))


if __name__ == "__main__":
  main()
