-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

target("fmia")
  set_kind("object")
  add_files("**.cppm", { public = true })
target_end()

add_includedirs("fmia")
