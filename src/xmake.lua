-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

add_includedirs("fmia")

target("fmia")
  set_kind("object")
  add_files("**.cppm", { public = true })
  add_defines("NDEBUG")
  set_optimize("fastest")
  add_cxxflags("-fcontract-evaluation-semantic=ignore")
target_end()

if is_mode("debug") then
  target("fmia.debug")
    set_kind("object")
    add_files("**.cppm", { public = true })
    set_optimize("none")
    add_cxxflags("-fcontract-evaluation-semantic=enforce")
  target_end()
end
