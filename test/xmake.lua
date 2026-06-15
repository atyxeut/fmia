-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

target("fmia.test")
  set_kind("object")
  add_deps("fmia");
  add_files("fmia/**.cppm", { public = true })
target_end()

target("any")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/any.cpp")
target_end()

target("1d_sparse_table_math_correctness")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/1d_sparse_table_math_correctness.cpp")
target_end()

target("int128_input.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/int128_input.cpp")
target_end()

target("int128_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/int128_output.cpp")
target_end()

target("naive_bigint_class_input.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_class_input.cpp")
target_end()

target("naive_bigint_class_ctor_and_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_class_ctor_and_output.cpp")
target_end()

target("naive_bigint_div.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_div.cpp")
target_end()

target("std_pair_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/util/std_pair_output.cpp")
target_end()

target("std_tuple_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/util/std_tuple_output.cpp")
target_end()

target("make_mdvector.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/util/make_mdvector.cpp")
target_end()

target("ranges_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/util/ranges_output.cpp")
target_end()
