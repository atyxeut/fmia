-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

target("fmia.test")
  set_kind("object")
  add_deps("fmia");
  add_files("fmia/**.cppm", { public = true })
target_end()

target("tmp.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/tmp.cpp")
target_end()

target("1d_sparse_table_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/1d_sparse_table_correctness.cpp")
target_end()

target("kmp_search_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/kmp_search_correctness.cpp")
target_end()

target("make_mdvector_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/make_mdvector_correctness.cpp")
target_end()

target("std_pair_output_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/std_pair_output_correctness.cpp")
target_end()

target("std_tuple_output_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/data_structure/std_tuple_output_correctness.cpp")
target_end()

target("ranges_output_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/io/ranges_output_correctness.cpp")
target_end()

target("int128_input_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/int128_input_correctness.cpp")
target_end()

target("int128_output_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/int128_output_correctness.cpp")
target_end()

target("naive_bigint_class_ctor_and_output_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_class_ctor_and_output_correctness.cpp")
target_end()

target("naive_bigint_class_input_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_class_input_correctness.cpp")
target_end()

target("naive_bigint_div_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/naive_bigint_div_correctness.cpp")
target_end()

target("print_first_10000_humble_numbers.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/math/print_first_10000_humble_numbers.cpp")
target_end()

target("internal_sorting_algorithms_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/sort/internal_sorting_algorithms_correctness.cpp")
target_end()

target("update_extrema_functions_correctness.test")
  set_kind("binary")
  add_deps("fmia.test")
  add_files("runner/util/update_extrema_functions_correctness.cpp")
target_end()
