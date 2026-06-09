-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

target("test-fmia")
  set_kind("object")
  add_deps("fmia");
  add_files("fmia/**.cppm", { public = true })
target_end()

target("test-any")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/any.cpp")
target_end()

target("test-1d_sparse_table_math_correctness")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/data_structure/1d_sparse_table_math_correctness.cpp")
target_end()

target("test-int128_input")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/int128_input.cpp")
target_end()

target("test-int128_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/int128_output.cpp")
target_end()

target("test-naive_bigint_class_input")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/naive_bigint_class_input.cpp")
target_end()

target("test-naive_bigint_class_ctor_and_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/naive_bigint_class_ctor_and_output.cpp")
target_end()

target("test-naive_bigint_div")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/naive_bigint_div.cpp")
target_end()

target("test-std_pair_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/std_pair_output.cpp")
target_end()

target("test-std_tuple_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/std_tuple_output.cpp")
target_end()

target("test-make_mdvector")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/make_mdvector.cpp")
target_end()

target("test-ranges_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/ranges_output.cpp")
target_end()
