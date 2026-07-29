-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

target("fmia.test")
  set_kind("object")
  add_deps("fmia");
  add_files("fmia/**.cppm", { public = true })
target_end()

target("temp.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/temp.cpp")
target_end()

target("1d_sparse_table_math_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/1d_sparse_table_math_correctness.cpp")
target_end()

target("make_mdvector.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/make_mdvector.cpp")
target_end()

target("std_pair_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/std_pair_output.cpp")
target_end()

target("std_tuple_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/std_tuple_output.cpp")
target_end()

target("ranges_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/io/ranges_output.cpp")
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

target("naive_bigint_class_ctor_and_output.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_class_ctor_and_output.cpp")
target_end()

target("naive_bigint_class_input.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_class_input.cpp")
target_end()

target("naive_bigint_div.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_div.cpp")
target_end()

target("print_first_10000_humble_numbers.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/print_first_10000_humble_numbers.cpp")
target_end()

target("sorting_algorithms_math_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/util/sorting_algorithms_math_correctness.cpp")
target_end()

target("update_extrema_functionality.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/util/update_extrema_functionality.cpp")
target_end()
