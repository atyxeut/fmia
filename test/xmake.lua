-- SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
-- SPDX-License-Identifier: GPL-3.0-or-later

set_optimize("none")
add_cxxflags("-fcontract-evaluation-semantic=enforce")

if is_mode("debug") then
  target("test")
    set_kind("object")
    add_deps("fmia.debug");
    add_files("fmia/**.cppm", { public = true })
  target_end()

  target("test.tmp")
    set_kind("binary")
    add_deps("test")
    add_files("runner/tmp.cpp")
  target_end()

  target("test.1d_sparse_table_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/data_structure/1d_sparse_table_correctness.cpp")
  target_end()

  target("test.kmp_search_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/data_structure/kmp_search_correctness.cpp")
  target_end()

  target("test.make_mdvector_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/data_structure/make_mdvector_correctness.cpp")
  target_end()

  target("test.std_pair_output_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/data_structure/std_pair_output_correctness.cpp")
  target_end()

  target("test.std_tuple_output_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/data_structure/std_tuple_output_correctness.cpp")
  target_end()

  target("test.ranges_output_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/io/ranges_output_correctness.cpp")
  target_end()

  target("test.int128_input_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/math/int128_input_correctness.cpp")
  target_end()

  target("test.int128_output_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/math/int128_output_correctness.cpp")
  target_end()

  target("test.naive_bigint_class_ctor_and_output_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/math/naive_bigint_class_ctor_and_output_correctness.cpp")
  target_end()

  target("test.naive_bigint_class_input_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/math/naive_bigint_class_input_correctness.cpp")
  target_end()

  target("test.naive_bigint_div_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/math/naive_bigint_div_correctness.cpp")
  target_end()

  target("test.print_first_10000_humble_numbers")
    set_kind("binary")
    add_deps("test")
    add_files("runner/math/print_first_10000_humble_numbers.cpp")
  target_end()

  target("test.internal_sorting_algorithms_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/sort/internal_sorting_algorithms_correctness.cpp")
  target_end()

  target("test.update_extrema_functions_correctness")
    set_kind("binary")
    add_deps("test")
    add_files("runner/util/update_extrema_functions_correctness.cpp")
  target_end()
end
