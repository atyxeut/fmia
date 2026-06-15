-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

target("fmia.benchmark")
  set_kind("object")
  add_deps("fmia");
  add_files("fmia/**.cppm", { public = true })
target_end()
