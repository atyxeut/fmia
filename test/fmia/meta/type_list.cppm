// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.meta.type_list;

import std;

import fmia.meta.type_list;

using namespace fmia::meta;

consteval void check_is_any_of() {
  static_assert(is_any_of_v<int> == false);
  static_assert(is_any_of_v<int, int> == true);
  static_assert(is_any_of_v<int, double> == false);
  static_assert(is_any_of_v<int, double, long double, char, const char, float, std::string, const int> == false);
  static_assert(is_any_of_v<int, double, long double, char, const char, int, std::string> == true);
}

consteval void check_is_none_of() {
  static_assert(is_none_of_v<int> == true);
  static_assert(is_none_of_v<int, int> == false);
  static_assert(is_none_of_v<int, double> == true);
  static_assert(is_none_of_v<int, double, long double, char, const char, float, std::string, const int> == true);
  static_assert(is_none_of_v<int, double, long double, char, const char, int, std::string> == false);
}

consteval void check_all_the_same() {
  static_assert(all_the_same_v<> == true);
  static_assert(all_the_same_v<int, int> == true);
  static_assert(all_the_same_v<int, double> == false);
  static_assert(all_the_same_v<int, int, int, int, int, const char, float, std::string> == false);
  static_assert(all_the_same_v<int, int, int, int, int, int, int, int> == true);
}

using type_list_0 = type_list<double, float, std::vector<int>, long long>;
using type_list_1 = type_list<std::string, unsigned, const volatile bool, bool>;
using type_list_2 = type_list<int, char, const int, volatile char>;
using type_list_3 = type_list<int, int, int, int, int, int, int>;
using type_list_4 = type_list<
  const float, float, double, int, const double, char, std::byte, volatile unsigned long long, volatile void, char, unsigned char,
  signed char, short, void, const void
>;
using type_list_5 = type_list<
  const float, float, double, int, const double, std::byte, volatile unsigned long long, volatile void, unsigned char, signed char, short,
  void, const void
>;

using indexed_type_list_0 = indexed_type_list<std::make_index_sequence<length_v<type_list_0>>, type_list_0>;
using indexed_type_list_1 = indexed_type_list<std::make_index_sequence<length_v<type_list_1>>, type_list_1>;
using indexed_type_list_2 = indexed_type_list<std::make_index_sequence<length_v<type_list_2>>, type_list_2>;
using indexed_type_list_3 = indexed_type_list<std::make_index_sequence<length_v<type_list_3>>, type_list_3>;
using indexed_type_list_4 = indexed_type_list<std::make_index_sequence<length_v<type_list_4>>, type_list_4>;
using indexed_type_list_5 = indexed_type_list<std::make_index_sequence<length_v<type_list_5>>, type_list_5>;

consteval void check_lookup() {
  static_assert(std::same_as<lookup_t<indexed_type_list_0, 2>, indexed_type<2, std::vector<int>>>);
  static_assert(std::same_as<lookup_t<indexed_type_list_1, 0>, indexed_type<0, std::string>>);
  static_assert(std::same_as<lookup_t<indexed_type_list_2, 1>, indexed_type<1, char>>);

  constexpr std::size_t tail_index = length_v<indexed_type_list_5> - 1;
  static_assert(std::same_as<lookup_t<indexed_type_list_5, tail_index>, indexed_type<tail_index, const void>>);
}

consteval void check_cons() {
  // cons for meta::type_list
  static_assert(std::same_as<cons_t<type_list_0, int>, type_list<int, double, float, std::vector<int>, long long>>);
  static_assert(std::same_as<cons_t<type_list_1, void>, type_list<void, std::string, unsigned, const volatile bool, bool>>);
  static_assert(std::same_as<cons_t<type_list_2, double>, type_list<double, int, char, const int, volatile char>>);

  // cons for meta::indexed_type_list
  using indexed_type_list_0_after_cons =
    indexed_type_list<std::index_sequence<4, 0, 1, 2, 3>, type_list<int, double, float, std::vector<int>, long long>>;
  static_assert(std::same_as<cons_t<indexed_type_list_0, indexed_type<4, int>>, indexed_type_list_0_after_cons>);

  using indexed_type_list_1_after_cons =
    indexed_type_list<std::index_sequence<4, 0, 1, 2, 3>, type_list<double, std::string, unsigned, const volatile bool, bool>>;
  static_assert(std::same_as<cons_t<indexed_type_list_1, indexed_type<4, double>>, indexed_type_list_1_after_cons>);

  using indexed_type_list_2_after_cons =
    indexed_type_list<std::index_sequence<9, 0, 1, 2, 3>, type_list<void, int, char, const int, volatile char>>;
  static_assert(std::same_as<cons_t<indexed_type_list_2, indexed_type<9, void>>, indexed_type_list_2_after_cons>);
}

consteval void check_snoc() {
  // snoc for meta::type_list
  static_assert(std::same_as<snoc_t<type_list_0, int>, type_list<double, float, std::vector<int>, long long, int>>);
  static_assert(std::same_as<snoc_t<type_list_1, void>, type_list<std::string, unsigned, const volatile bool, bool, void>>);
  static_assert(std::same_as<snoc_t<type_list_2, double>, type_list<int, char, const int, volatile char, double>>);

  // snoc for meta::indexed_type_list
  using indexed_type_list_0_after_snoc =
    indexed_type_list<std::index_sequence<0, 1, 2, 3, 4>, type_list<double, float, std::vector<int>, long long, int>>;
  static_assert(std::same_as<snoc_t<indexed_type_list_0, indexed_type<4, int>>, indexed_type_list_0_after_snoc>);

  using indexed_type_list_1_after_snoc =
    indexed_type_list<std::index_sequence<0, 1, 2, 3, 4>, type_list<std::string, unsigned, const volatile bool, bool, double>>;
  static_assert(std::same_as<snoc_t<indexed_type_list_1, indexed_type<4, double>>, indexed_type_list_1_after_snoc>);

  using indexed_type_list_2_after_snoc =
    indexed_type_list<std::index_sequence<0, 1, 2, 3, 9>, type_list<int, char, const int, volatile char, void>>;
  static_assert(std::same_as<snoc_t<indexed_type_list_2, indexed_type<9, void>>, indexed_type_list_2_after_snoc>);
}

consteval void check_replicate() {
  static_assert(std::same_as<replicate_t<void, 0>, empty_type_list>);

  using int_replicated_5_times_type_list = type_list<int, int, int, int, int>;
  static_assert(std::same_as<replicate_t<int, 5>, int_replicated_5_times_type_list>);
}

template <list_of_types>
struct huge_concat_test_helper;

template <list_of_types... Ts>
struct huge_concat_test_helper<type_list<Ts...>> : concat<Ts...> {};

template <list_of_types T>
using huge_concat_test_helper_t = huge_concat_test_helper<T>::type;

consteval void check_concat() {
  static_assert(std::same_as<concat_t<empty_type_list>, empty_type_list>);
  static_assert(std::same_as<concat_t<type_list_1>, type_list_1>);

  static_assert(std::same_as<concat_t<empty_indexed_type_list>, empty_indexed_type_list>);
  static_assert(std::same_as<concat_t<indexed_type_list_1>, indexed_type_list_1>);

  using concatenation_of_1_2 = type_list<double, float, std::vector<int>, long long, std::string, unsigned, const volatile bool, bool>;
  static_assert(std::same_as<concat_t<type_list_0, type_list_1>, concatenation_of_1_2>);

  using concatenation_of_1_2_3 = type_list<
    double, float, std::vector<int>, long long, std::string, unsigned, const volatile bool, bool, int, char, const int, volatile char
  >;
  static_assert(std::same_as<concat_t<type_list_0, type_list_1, type_list_2>, concatenation_of_1_2_3>);

  using concatenation_of_1_2_3_4 = type_list<
    double, float, std::vector<int>, long long, std::string, unsigned, const volatile bool, bool, int, char, const int, volatile char, int,
    int, int, int, int, int, int
  >;
  static_assert(std::same_as<concat_t<type_list_0, type_list_1, type_list_2, type_list_3>, concatenation_of_1_2_3_4>);

  // test `replicate` complexity, length_v<type_list_3> * 1500 recursion depth (for a O(N) recursive implementation)
  // will make the compiler crash by default
  using huge_concatenation_before = replicate_t<type_list_3, 1500>;
  using huge_concatenation_after = replicate_t<int, length_v<type_list_3> * 1500>;

  // test `concat` complexity, 1500 recursion depth (for a O(n) recursive implementation) will make the compiler crash
  // by default
  static_assert(std::same_as<huge_concat_test_helper_t<huge_concatenation_before>, huge_concatenation_after>);

  using list_1 = indexed_type_list<std::index_sequence<0, 1, 2, 3, 4>, type_list<double, float, std::vector<int>, long long, std::string>>;
  using list_2 = indexed_type_list<std::index_sequence<5, 8, 9>, type_list<unsigned, const volatile bool, bool>>;
  using list_3 = indexed_type_list<std::index_sequence<0, 1, 2>, type_list<int, bool, void>>;

  using indexed_concatenation_of_1_2 = indexed_type_list<
    std::index_sequence<0, 1, 2, 3, 4, 5, 8, 9>,
    type_list<double, float, std::vector<int>, long long, std::string, unsigned, const volatile bool, bool>
  >;
  static_assert(std::same_as<concat_t<list_1, list_2>, indexed_concatenation_of_1_2>);

  using indexed_concatenation_of_2_1 = indexed_type_list<
    std::index_sequence<5, 8, 9, 0, 1, 2, 3, 4>,
    type_list<unsigned, const volatile bool, bool, double, float, std::vector<int>, long long, std::string>
  >;
  static_assert(std::same_as<concat_t<list_2, list_1>, indexed_concatenation_of_2_1>);

  // check SFINAE functionality
  auto concat_with_duplication_fails = []<typename List1, typename List2> consteval {
    return !(requires { typename concat_t<List1, List2>; });
  };
  static_assert(concat_with_duplication_fails.operator ()<list_1, list_3>());
}

consteval void check_reverse() {
  using reversed_empty_type_list = empty_type_list;
  static_assert(std::same_as<reverse_t<empty_type_list>, reversed_empty_type_list>);

  using reversed_type_list_of_0 = type_list<long long, std::vector<int>, float, double>;
  static_assert(std::same_as<reverse_t<type_list_0>, reversed_type_list_of_0>);

  using reversed_type_list_of_1 = type_list<bool, const volatile bool, unsigned, std::string>;
  static_assert(std::same_as<reverse_t<type_list_1>, reversed_type_list_of_1>);
}

consteval void check_init() {
  // force a template substitution to make `requires` work
  auto test_empty_list = []<typename T> consteval {
    return requires { typename init_t<T>; };
  };
  static_assert(test_empty_list.operator ()<empty_type_list>() == false);
  static_assert(test_empty_list.operator ()<empty_indexed_type_list>() == false);

  using init_type_list_of_0 = type_list<double, float, std::vector<int>>;
  static_assert(std::same_as<init_t<type_list_0>, init_type_list_of_0> == true);

  using init_type_list_of_1 = type_list<std::string, unsigned, const volatile bool>;
  static_assert(std::same_as<init_t<type_list_1>, init_type_list_of_1> == true);

  using init_indexed_type_list_of_0 = indexed_type_list<std::index_sequence<0, 1, 2>, type_list<double, float, std::vector<int>>>;
  static_assert(std::same_as<init_t<indexed_type_list_0>, init_indexed_type_list_of_0> == true);

  using init_indexed_type_list_of_1 =
    indexed_type_list<std::index_sequence<0, 1, 2>, type_list<std::string, unsigned, const volatile bool>>;
  static_assert(std::same_as<init_t<indexed_type_list_1>, init_indexed_type_list_of_1> == true);
}

consteval void check_take() {
  using take_0_type_list_of_empty = empty_type_list;
  static_assert(std::same_as<take_t<empty_type_list, 0>, take_0_type_list_of_empty>);

  using take_0_type_list_of_nonempty = empty_type_list;
  static_assert(std::same_as<take_t<type_list_0, 0>, take_0_type_list_of_nonempty>);

  using take_3_type_list_of_0 = type_list<double, float, std::vector<int>>;
  static_assert(std::same_as<take_t<type_list_0, 3>, take_3_type_list_of_0>);

  using take_2_type_list_of_1 = type_list<std::string, unsigned>;
  static_assert(std::same_as<take_t<type_list_1, 2>, take_2_type_list_of_1>);

  using take_6_type_list_of_4 = type_list<const float, float, double, int, const double, char>;
  static_assert(std::same_as<take_t<type_list_4, 6>, take_6_type_list_of_4>);
}

consteval void check_take_end() {
  using take_0_type_list_of_empty_from_end = empty_type_list;
  static_assert(std::same_as<take_end_t<empty_type_list, 0>, take_0_type_list_of_empty_from_end>);

  using take_0_type_list_of_nonempty_from_end = empty_type_list;
  static_assert(std::same_as<take_end_t<type_list_0, 0>, take_0_type_list_of_nonempty_from_end>);

  using take_3_type_list_of_0_from_end = type_list<float, std::vector<int>, long long>;
  static_assert(std::same_as<take_end_t<type_list_0, 3>, take_3_type_list_of_0_from_end>);

  using take_2_type_list_of_1_from_end = type_list<const volatile bool, bool>;
  static_assert(std::same_as<take_end_t<type_list_1, 2>, take_2_type_list_of_1_from_end>);

  using take_6_type_list_of_4_from_end = type_list<char, unsigned char, signed char, short, void, const void>;
  static_assert(std::same_as<take_end_t<type_list_4, 6>, take_6_type_list_of_4_from_end>);
}

consteval void check_drop() {
  using dropped_0_type_list_of_empty = empty_type_list;
  static_assert(std::same_as<drop_t<empty_type_list, 0>, dropped_0_type_list_of_empty>);

  using dropped_0_type_list_of_0 = type_list<double, float, std::vector<int>, long long>;
  static_assert(std::same_as<drop_t<type_list_0, 0>, dropped_0_type_list_of_0>);

  using dropped_0_type_list_of_1 = type_list<std::string, unsigned, const volatile bool, bool>;
  static_assert(std::same_as<drop_t<type_list_1, 0>, dropped_0_type_list_of_1>);

  using dropped_3_type_list_of_0 = type_list<long long>;
  static_assert(std::same_as<drop_t<type_list_0, 3>, dropped_3_type_list_of_0>);

  using dropped_3_type_list_of_1 = type_list<bool>;
  static_assert(std::same_as<drop_t<type_list_1, 3>, dropped_3_type_list_of_1>);
}

consteval void check_drop_end() {
  using dropped_0_type_list_of_empty_from_end = empty_type_list;
  static_assert(std::same_as<drop_end_t<empty_type_list, 0>, dropped_0_type_list_of_empty_from_end>);

  using dropped_0_type_list_of_0_from_end = type_list<double, float, std::vector<int>, long long>;
  static_assert(std::same_as<drop_end_t<type_list_0, 0>, dropped_0_type_list_of_0_from_end>);

  using dropped_0_type_list_of_1_from_end = type_list<std::string, unsigned, const volatile bool, bool>;
  static_assert(std::same_as<drop_end_t<type_list_1, 0>, dropped_0_type_list_of_1_from_end>);

  using dropped_3_type_list_of_0_from_end = type_list<double>;
  static_assert(std::same_as<drop_end_t<type_list_0, 3>, dropped_3_type_list_of_0_from_end>);

  using dropped_3_type_list_of_1_from_end = type_list<std::string>;
  static_assert(std::same_as<drop_end_t<type_list_1, 3>, dropped_3_type_list_of_1_from_end>);

  using dropped_2_type_list_of_2_from_end = type_list<int, char>;
  static_assert(std::same_as<drop_end_t<type_list_2, 2>, dropped_2_type_list_of_2_from_end>);
}

consteval void check_take_while() {
  using take_while_type_list_of_empty_list = empty_type_list;
  static_assert(std::same_as<take_while_t<empty_type_list, std::is_integral>, take_while_type_list_of_empty_list>);

  using take_while_type_list_of_0 = empty_type_list;
  static_assert(std::same_as<take_while_t<type_list_0, std::is_integral>, take_while_type_list_of_0>);

  using take_while_type_list_of_1 = empty_type_list;
  static_assert(std::same_as<take_while_t<type_list_1, std::is_integral>, take_while_type_list_of_1>);

  using take_while_type_list_of_2 = type_list<int, char, const int, volatile char>;
  static_assert(std::same_as<take_while_t<type_list_2, std::is_integral>, take_while_type_list_of_2>);

  using take_while_type_list_of_3 = empty_type_list;
  static_assert(std::same_as<take_while_t<type_list_3, std::is_floating_point>, take_while_type_list_of_3>);

  using take_while_type_list_of_4_1 = empty_type_list;
  static_assert(std::same_as<take_while_t<type_list_4, std::is_void>, take_while_type_list_of_4_1>);

  using take_while_type_list_of_4_2 = type_list<const float, float, double>;
  static_assert(std::same_as<take_while_t<type_list_4, std::is_floating_point>, take_while_type_list_of_4_2>);
}

consteval void check_take_while_end() {
  using take_while_type_list_of_empty_list_from_end = empty_type_list;
  static_assert(std::same_as<take_while_end_t<empty_type_list, std::is_integral>, take_while_type_list_of_empty_list_from_end>);

  using take_while_type_list_of_0_from_end = type_list<long long>;
  static_assert(std::same_as<take_while_end_t<type_list_0, std::is_integral>, take_while_type_list_of_0_from_end>);

  using take_while_type_list_of_1_from_end = type_list<unsigned, const volatile bool, bool>;
  static_assert(std::same_as<take_while_end_t<type_list_1, std::is_integral>, take_while_type_list_of_1_from_end>);

  using take_while_type_list_of_2_from_end = type_list<int, char, const int, volatile char>;
  static_assert(std::same_as<take_while_end_t<type_list_2, std::is_integral>, take_while_type_list_of_2_from_end>);

  using take_while_type_list_of_3_from_end = empty_type_list;
  static_assert(std::same_as<take_while_end_t<type_list_3, std::is_floating_point>, take_while_type_list_of_3_from_end>);

  using take_while_type_list_of_4_1_from_end = type_list<void, const void>;
  static_assert(std::same_as<take_while_end_t<type_list_4, std::is_void>, take_while_type_list_of_4_1_from_end>);

  using take_while_type_list_of_4_2_from_end = empty_type_list;
  static_assert(std::same_as<take_while_end_t<type_list_4, std::is_floating_point>, take_while_type_list_of_4_2_from_end>);
}

consteval void check_drop_while() {
  using drop_while_type_list_of_0 = type_list<double, float, std::vector<int>, long long>;
  static_assert(std::same_as<drop_while_t<type_list_0, std::is_integral>, drop_while_type_list_of_0>);

  using drop_while_type_list_of_1 = type_list<std::string, unsigned, const volatile bool, bool>;
  static_assert(std::same_as<drop_while_t<type_list_1, std::is_integral>, drop_while_type_list_of_1>);

  using drop_while_type_list_of_2 = empty_type_list;
  static_assert(std::same_as<drop_while_t<type_list_2, std::is_integral>, drop_while_type_list_of_2>);

  using drop_while_type_list_of_3 = type_list<int, int, int, int, int, int, int>;
  static_assert(std::same_as<drop_while_t<type_list_3, std::is_void>, drop_while_type_list_of_3>);

  using drop_while_type_list_of_4 = type_list<
    int, const double, char, std::byte, volatile unsigned long long, volatile void, char, unsigned char, signed char, short, void,
    const void
  >;
  static_assert(std::same_as<drop_while_t<type_list_4, std::is_floating_point>, drop_while_type_list_of_4>);
}

consteval void check_drop_while_end() {
  using drop_while_type_list_of_0_from_end = type_list<double, float, std::vector<int>>;
  static_assert(std::same_as<drop_while_end_t<type_list_0, std::is_integral>, drop_while_type_list_of_0_from_end>);

  using drop_while_type_list_of_1_from_end = type_list<std::string>;
  static_assert(std::same_as<drop_while_end_t<type_list_1, std::is_integral>, drop_while_type_list_of_1_from_end>);

  using drop_while_type_list_of_2_from_end = empty_type_list;
  static_assert(std::same_as<drop_while_end_t<type_list_2, std::is_integral>, drop_while_type_list_of_2_from_end>);

  using drop_while_type_list_of_3_from_end = type_list<int, int, int, int, int, int, int>;
  static_assert(std::same_as<drop_while_end_t<type_list_3, std::is_void>, drop_while_type_list_of_3_from_end>);

  using drop_while_type_list_of_4_from_end = type_list<
    const float, float, double, int, const double, char, std::byte, volatile unsigned long long, volatile void, char, unsigned char,
    signed char, short, void, const void
  >;
  static_assert(std::same_as<drop_while_end_t<type_list_4, std::is_floating_point>, drop_while_type_list_of_4_from_end>);
}

consteval void check_filter() {
  using filtered_type_list_of_4_1 = type_list<int, char, volatile unsigned long long, char, unsigned char, signed char, short>;
  static_assert(std::same_as<filter_t<type_list_4, std::is_integral>, filtered_type_list_of_4_1>);

  using filtered_type_list_of_4_2 = type_list<const float, float, double, const double>;
  static_assert(std::same_as<filter_t<type_list_4, std::is_floating_point>, filtered_type_list_of_4_2>);

  using filtered_type_list_of_4_3 = type_list<volatile void, void, const void>;
  static_assert(std::same_as<filter_t<type_list_4, std::is_void>, filtered_type_list_of_4_3>);

  using filtered_type_list_of_4_4 = type_list<volatile unsigned long long, unsigned char>;
  static_assert(std::same_as<filter_t<type_list_4, std::is_unsigned>, filtered_type_list_of_4_4>);

  using filtered_type_list_of_5 = type_list<const float, float, double, int, const double, signed char, short>;
  static_assert(std::same_as<filter_t<type_list_5, std::is_signed>, filtered_type_list_of_5>);
}

consteval void check_keep() {
  using keep_only_int_type_list_of_empty = empty_type_list;
  static_assert(std::same_as<keep_t<empty_type_list, int>, keep_only_int_type_list_of_empty>);

  using keep_only_int_type_list_of_0 = empty_type_list;
  static_assert(std::same_as<keep_t<type_list_0, int>, keep_only_int_type_list_of_0>);

  using keep_only_int_type_list_of_2 = type_list<int>;
  static_assert(std::same_as<keep_t<type_list_2, int>, keep_only_int_type_list_of_2>);

  using keep_only_int_type_list_of_3 = type_list<int, int, int, int, int, int, int>;
  static_assert(std::same_as<keep_t<type_list_3, int>, keep_only_int_type_list_of_3>);

  using keep_only_char_type_list_of_4 = type_list<char, char>;
  static_assert(std::same_as<keep_t<type_list_4, char>, keep_only_char_type_list_of_4>);
}

consteval void check_remove() {
  using remove_all_int_type_list_of_empty = empty_type_list;
  static_assert(std::same_as<remove_t<empty_type_list, int>, remove_all_int_type_list_of_empty>);

  using remove_all_int_type_list_of_0 = type_list<double, float, std::vector<int>, long long>;
  static_assert(std::same_as<remove_t<type_list_0, int>, remove_all_int_type_list_of_0>);

  using remove_all_int_type_list_of_2 = type_list<char, const int, volatile char>;
  static_assert(std::same_as<remove_t<type_list_2, int>, remove_all_int_type_list_of_2>);

  using remove_all_int_type_list_of_3 = empty_type_list;
  static_assert(std::same_as<remove_t<type_list_3, int>, remove_all_int_type_list_of_3>);

  using remove_all_char_type_list_of_4 = type_list<
    const float, float, double, int, const double, std::byte, volatile unsigned long long, volatile void, unsigned char, signed char, short,
    void, const void
  >;
  static_assert(std::same_as<remove_t<type_list_4, char>, remove_all_char_type_list_of_4>);
}

consteval void check_transform() {
  using type_list_0_after_add_const = type_list<const double, const float, const std::vector<int>, const long long>;
  static_assert(std::same_as<transform_t<type_list_0, std::add_const>, type_list_0_after_add_const>);

  using type_list_3_after_make_unsigned =
    type_list<unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>;
  static_assert(std::same_as<transform_t<type_list_3, std::make_unsigned>, type_list_3_after_make_unsigned>);
}

consteval void check_any() {
  static_assert(any_v<empty_type_list, std::is_integral> == false);
  static_assert(any_v<type_list_0, std::is_integral> == true);
  static_assert(any_v<type_list_1, std::is_floating_point> == false);
  static_assert(any_v<type_list_2, std::is_floating_point> == false);
  static_assert(any_v<type_list_3, std::is_unsigned> == false);
  static_assert(any_v<type_list_4, std::is_unsigned> == true);
}

consteval void check_none() {
  static_assert(none_v<empty_type_list, std::is_integral> == true);
  static_assert(none_v<type_list_0, std::is_integral> == false);
  static_assert(none_v<type_list_1, std::is_floating_point> == true);
  static_assert(none_v<type_list_2, std::is_floating_point> == true);
  static_assert(none_v<type_list_3, std::is_unsigned> == true);
  static_assert(none_v<type_list_4, std::is_signed> == false);
}

consteval void check_all() {
  static_assert(all_v<empty_type_list, std::is_integral> == false);
  static_assert(all_v<type_list_0, std::is_integral> == false);
  static_assert(all_v<type_list_1, std::is_floating_point> == false);
  static_assert(all_v<type_list_2, std::is_floating_point> == false);
  static_assert(all_v<type_list_3, std::is_unsigned> == false);
  static_assert(all_v<type_list_4, std::is_signed> == false);
}

consteval void check_has_any() {
  static_assert(has_any_v<empty_type_list, double> == false);
  static_assert(has_any_v<type_list_0, double> == true);
  static_assert(has_any_v<type_list_1, int> == false);
  static_assert(has_any_v<indexed_type_list_0, double> == true);
  static_assert(has_any_v<indexed_type_list_1, int> == false);
}

consteval void check_has_none() {
  static_assert(has_none_v<empty_type_list, double> == true);
  static_assert(has_none_v<type_list_0, double> == false);
  static_assert(has_none_v<type_list_1, int> == true);
  static_assert(has_none_v<indexed_type_list_0, double> == false);
  static_assert(has_none_v<indexed_type_list_1, int> == true);
}

consteval void check_is_predicate() {
  // test for templates that accept 1 template parameter
  static_assert(is_predicate_v<std::is_integral> == true);

  // test for templates that accept more than 1 (but fixed) template parameter
  static_assert(is_predicate_v<std::is_same> == true);

  // test for templates that accept variadic template parameters
  static_assert(is_predicate_v<std::is_constructible> == true);

  // test for templates whose `value` member does not have type bool
  static_assert(is_predicate_v<std::tuple_size> == false);

  // test for templates that do not have a `value` member
  static_assert(is_predicate_v<std::add_const> == false);

  // test if `wrapped_predicate` concept works
  static_assert(predicate<std::is_integral> == true);
  static_assert(wrapped_predicate<template_wrapper<std::is_integral>> == true);
  static_assert(predicate<std::is_same> == true);
  static_assert(wrapped_predicate<template_wrapper<std::is_same>> == true);
  static_assert(predicate<std::add_const> == false);
  static_assert(wrapped_predicate<template_wrapper<std::add_const>> == false);
}
