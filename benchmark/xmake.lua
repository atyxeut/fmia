-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

add_defines("NDEBUG")
set_optimize("fastest")
add_cxxflags("-fcontract-evaluation-semantic=ignore")

if is_mode("debug") then
  target("benchmark")
    set_kind("object")
    add_deps("fmia");
    add_files("fmia/**.cppm", { public = true })
  target_end()

  target("benchmark.hoare_quick_sort_variations")
    set_kind("binary")
    add_deps("benchmark")
    add_files("runner/sort/hoare_quick_sort_variations.cpp")
  target_end()
end
