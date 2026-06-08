-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

set_xmakever("3.0.9")
set_project("fast-modern-implementations-of-algorithms")
set_languages("cxx26")

add_rules("mode.debug", "mode.release")
set_policy("check.auto_ignore_flags", false)

if is_mode("release") then
  add_defines("NDEBUG")
end

set_config("builddir", "build")
set_config("runtimes", "c++_shared")

if is_host("windows") then
  -- recommend mingw llvm binary: https://github.com/mstorsjo/llvm-mingw
  -- recommend mingw gcc binary: https://gcc-mcf.lhmouse.com/
  set_config("plat", "mingw")
  if get_config("toolchain") == "gcc" then
    -- gcc 16 still requires this on Windows
    add_syslinks("stdc++exp")
    add_cxxflags("-fcontracts", "-freflection")
  end
elseif is_host("macosx") then
  if get_config("toolchain") == "gcc" then
    -- gcc-15 from Homebrew
    set_toolset("cxx", "/usr/local/opt/gcc@15/bin/g++-15") -- `usr` is the username
    set_toolset("ld", "/usr/local/opt/gcc@15/bin/g++-15")  -- `usr` is the username
  elseif get_config("toolchain") == "llvm" then
    set_policy("build.c++.modules", true)
    -- binary from Homebrew: brew install llvm
    add_ldflags("-L/usr/local/opt/llvm@21/lib/c++")
  end
end

includes("src")
includes("test")
includes("benchmark")
