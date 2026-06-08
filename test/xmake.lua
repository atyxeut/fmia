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

target("test-data_structure-sparse_table-math_correctness")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/data_structure/sparse_table/math_correctness.cpp")
target_end()

target("test-math-int128_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/integer/fixed_precision/int128_output.cpp")
target_end()

target("test-math-int128_input")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/integer/fixed_precision/int128_input.cpp")
target_end()

target("test-math-bigint_naive_class_ctor_and_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/integer/arbitrary_precision/naive_class_ctor_and_output.cpp")
target_end()

target("test-math-bigint_naive_class_input")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/integer/arbitrary_precision/naive_class_input.cpp")
target_end()

target("test-math-bigint_naive_div")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/math/integer/arbitrary_precision/naive_div.cpp")
target_end()

target("test-util-std_pair_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/std_container_extension/std_pair_output.cpp")
target_end()

target("test-util-std_tuple_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/std_container_extension/std_tuple_output.cpp")
target_end()

target("test-util-make_vector")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/std_container_extension/make_vector.cpp")
target_end()

target("test-util-ranges_output")
  set_kind("binary")
  add_deps("test-fmia")
  add_files("runner/util/std_container_extension/ranges_output.cpp")
target_end()
