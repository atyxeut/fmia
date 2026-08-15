// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.meta.type_list;

import std;

import fmia.meta.integer_sequence;
import fmia.meta.not_same;

export namespace fmia::meta {

template <typename T, typename... Us>
struct is_any_of : std::disjunction<is_any_of<T, Us>...> {};

template <typename T, typename U>
struct is_any_of<T, U> : std::bool_constant<std::same_as<T, U>> {};

template <typename T, typename... Us>
inline constexpr bool is_any_of_v = is_any_of<T, Us...>::value;

template <typename T, typename... Us>
using is_none_of = std::negation<is_any_of<T, Us...>>;

template <typename T, typename... Us>
inline constexpr bool is_none_of_v = is_none_of<T, Us...>::value;

template <typename...>
struct all_the_same : std::true_type {};

template <typename T, typename... Us>
struct all_the_same<T, Us...> : std::conjunction<all_the_same<T, Us>...> {};

template <typename T, typename U>
struct all_the_same<T, U> : std::bool_constant<std::same_as<T, U>> {};

template <typename... Ts>
inline constexpr bool all_the_same_v = all_the_same<Ts...>::value;

} // export namespace fmia::meta

export namespace fmia::meta {

template <typename...>
struct type_list {};

template <typename>
struct is_no_cv_type_list : std::false_type {};

template <typename... Ts>
struct is_no_cv_type_list<type_list<Ts...>> : std::true_type {};

template <typename T>
inline constexpr bool is_no_cv_type_list_v = is_no_cv_type_list<T>::value;

template <typename T>
using is_type_list = is_no_cv_type_list<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_type_list_v = is_type_list<T>::value;

using empty_type_list = type_list<>;

template <typename>
struct is_no_cv_empty_type_list : std::false_type {};

template <>
struct is_no_cv_empty_type_list<empty_type_list> : std::true_type {};

template <typename T>
inline constexpr bool is_no_cv_empty_type_list_v = is_no_cv_empty_type_list<T>::value;

template <typename T>
using is_empty_type_list = is_no_cv_empty_type_list<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_empty_type_list_v = is_empty_type_list<T>::value;

template <std::size_t I, typename T>
struct indexed_type {
  static constexpr auto idx = I;
  using type = T;
};

template <std::size_t I, typename T>
inline constexpr auto indexed_type_v = indexed_type<I, T>::idx;

template <std::size_t I, typename T>
using indexed_type_t = indexed_type<I, T>::type;

template <typename>
struct is_no_cv_indexed_type : std::false_type {};

template <std::size_t I, typename T>
struct is_no_cv_indexed_type<indexed_type<I, T>> : std::true_type {};

template <typename T>
inline constexpr bool is_no_cv_indexed_type_v = is_no_cv_indexed_type<T>::value;

template <typename T>
using is_indexed_type = is_no_cv_indexed_type<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_indexed_type_v = is_indexed_type<T>::value;

template <typename, typename>
struct indexed_type_list;

template <std::size_t... Is, typename... Ts>
struct indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>> : indexed_type<Is, Ts>... {
  using indices = std::index_sequence<Is...>;
  using types = type_list<Ts...>;
};

template <typename>
struct is_no_cv_indexed_type_list : std::false_type {};

template <std::size_t... Is, typename... Ts>
struct is_no_cv_indexed_type_list<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>> : std::true_type {};

template <typename T>
inline constexpr bool is_no_cv_indexed_type_list_v = is_no_cv_indexed_type_list<T>::value;

template <typename T>
using is_indexed_type_list = is_no_cv_indexed_type_list<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_indexed_type_list_v = is_indexed_type_list<T>::value;

using empty_indexed_type_list = indexed_type_list<std::index_sequence<>, type_list<>>;

template <typename>
struct is_no_cv_empty_indexed_type_list : std::false_type {};

template <>
struct is_no_cv_empty_indexed_type_list<empty_indexed_type_list> : std::true_type {};

template <typename T>
inline constexpr bool is_no_cv_empty_indexed_type_list_v = is_no_cv_empty_indexed_type_list<T>::value;

template <typename T>
using is_empty_indexed_type_list = is_no_cv_empty_indexed_type_list<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_empty_indexed_type_list_v = is_empty_indexed_type_list<T>::value;

template <typename T>
concept list_of_types = is_type_list_v<T> || is_indexed_type_list_v<T>;

template <typename T>
concept empty_list_of_types = is_empty_type_list_v<T> || is_empty_indexed_type_list_v<T>;

template <typename T>
concept nonempty_list_of_types = list_of_types<T> && !empty_list_of_types<T>;

// name after Haskell Data.List length
// get the length of a type list
// O(1) time complexity
template <typename>
struct length;

template <typename... Ts>
struct length<type_list<Ts...>> : index_constant<sizeof...(Ts)> {};

template <std::size_t... Is, typename... Ts>
struct length<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>> : index_constant<sizeof...(Ts)> {};

template <typename AnyTypeList>
constexpr std::size_t length_v = length<AnyTypeList>::value;

// get the nth type of a type list (index starts at 0)
// O(1) time complexity
template <typename, std::size_t>
struct nth;

template <typename... Ts, std::size_t Idx>
struct nth<type_list<Ts...>, Idx> {
  using type = Ts...[Idx];
};

template <std::size_t... Is, typename... Ts, std::size_t Idx>
struct nth<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>, Idx> {
  using type = indexed_type<Is...[Idx], Ts...[Idx]>;
};

template <typename AnyTypeList, std::size_t Idx>
using nth_t = nth<AnyTypeList, Idx>::type;

// name after Haskell Data.List head
// get the first type of a type list
// O(1) time complexity
template <nonempty_list_of_types T>
using head = nth<T, 0>;

template <nonempty_list_of_types T>
using head_t = head<T>::type;

// name after Haskell Data.List last
// get the last type of a type list
// O(1) time complexity
template <nonempty_list_of_types T>
using last = nth<T, length_v<T> - 1>;

template <nonempty_list_of_types T>
using last_t = last<T>::type;

} // export namespace fmia::meta

namespace fmia::meta {

template <std::size_t I, typename T>
indexed_type<I, T> indexed_type_lookup_helper(indexed_type<I, T>);

} // namespace fmia::meta

export namespace fmia::meta {

// name after Haskell Data.List lookup
// get the corresponding indexed type of a indexed_type_list with a given index
// O(1) time complexity
template <typename T, std::size_t I>
  requires is_indexed_type_list_v<T>
struct lookup {
  using type = decltype(indexed_type_lookup_helper<I>(std::declval<T>()));
};

template <typename IndexedTypeList, std::size_t I>
using lookup_t = lookup<IndexedTypeList, I>::type;

// name after Haskell Data.List : operator (1 : [1, 2] --> [1, 1, 2])
// get a type list that has one element added to the beginning comparing to the given list
// O(1) time complexity for type_list
// O(n) time complexity for indexed_type_list (because of the validation for indices), where n is the length of the given list
template <typename, typename>
struct cons;

template <typename... Ts, typename T>
struct cons<type_list<Ts...>, T> {
  using type = type_list<T, Ts...>;
};

template <std::size_t... Is, typename... Ts, typename T>
  requires (is_indexed_type_v<T> && ((T::idx != Is) && ...))
struct cons<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>, T> {
  using type = indexed_type_list<std::index_sequence<T::idx, Is...>, type_list<typename T::type, Ts...>>;
};

template <typename AnyTypeList, typename T>
using cons_t = cons<AnyTypeList, T>::type;

template <typename AnyTypeList, typename T, bool B>
struct cons_if : std::conditional<B, cons_t<AnyTypeList, T>, AnyTypeList> {};

template <typename AnyTypeList, typename T, bool B>
using cons_if_t = cons_if<AnyTypeList, T, B>::type;

// get a type list that has one element added to the end comparing to the given list
// O(1) time complexity for type_list
// O(n) time complexity for indexed_type_list (because of the validation for indices), where n is the length of the given list
template <typename, typename>
struct snoc;

template <typename... Ts, typename T>
struct snoc<type_list<Ts...>, T> {
  using type = type_list<Ts..., T>;
};

template <std::size_t... Is, typename... Ts, typename T>
  requires (is_indexed_type_v<T> && ((T::idx != Is) && ...))
struct snoc<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>, T> {
  using type = indexed_type_list<std::index_sequence<Is..., T::idx>, type_list<Ts..., typename T::type>>;
};

template <typename AnyTypeList, typename T>
using snoc_t = snoc<AnyTypeList, T>::type;

template <typename AnyTypeList, typename T, bool B>
struct snoc_if : std::conditional<B, snoc_t<AnyTypeList, T>, AnyTypeList> {};

template <typename AnyTypeList, typename T, bool B>
using snoc_if_t = snoc_if<AnyTypeList, T, B>::type;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename, typename>
struct replicate_impl;

template <typename T, std::size_t... Is>
struct replicate_impl<T, std::index_sequence<Is...>> {
  using type = type_list<std::enable_if_t<(Is >= 0), T>...>;
};

} // namespace fmia::meta

export namespace fmia::meta {

// name after Haskell Data.List replicate
// get a type list that contains N identical types
// O(1) time complexity
template <typename T, std::size_t N>
using replicate = replicate_impl<T, std::make_index_sequence<N>>;

template <typename T, std::size_t N>
using replicate_t = replicate<T, N>::type;

template <template <typename...> typename T, typename... BoundArgs>
struct bind_front {
  template <typename... CallArgs>
  using type = T<BoundArgs..., CallArgs...>;
};

template <template <typename...> typename T, typename... BoundArgs>
struct bind_back {
  template <typename... CallArgs>
  using type = T<CallArgs..., BoundArgs...>;
};

template <template <typename...> typename T>
struct template_wrapper {
  template <typename... Args>
  using type = T<Args...>;
};

template <typename>
struct is_no_cv_template_wrapper : std::false_type {};

template <template <typename...> typename T>
struct is_no_cv_template_wrapper<template_wrapper<T>> : std::true_type {};

template <template <typename...> typename T, typename... BoundArgs>
struct is_no_cv_template_wrapper<bind_front<T, BoundArgs...>> : std::true_type {};

template <template <typename...> typename T, typename... BoundArgs>
struct is_no_cv_template_wrapper<bind_back<T, BoundArgs...>> : std::true_type {};

template <typename T>
inline constexpr bool is_no_cv_template_wrapper_v = is_no_cv_template_wrapper<T>::value;

template <typename T>
using is_template_wrapper = is_no_cv_template_wrapper<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_template_wrapper_v = is_template_wrapper<T>::value;

template <typename T>
concept wrapped_template = is_template_wrapper_v<T>;

template <wrapped_template T, typename... Args>
using invoke = T::template type<Args...>;

template <typename WrappedTemplate, typename... Args>
using invoke_t = invoke<WrappedTemplate, Args...>::type;

template <typename WrappedTemplate, typename... Args>
constexpr auto invoke_v = invoke<WrappedTemplate, Args...>::value;

} // export namespace fmia::meta

namespace fmia::meta {

template <template <typename...> typename, typename>
struct is_predicate_tester;

// clang-format off

template <template <typename...> typename Fn, typename... Ts>
struct is_predicate_tester<Fn, type_list<Ts...>> : std::bool_constant<requires { { Fn<Ts...>::value } -> no_cvref_same_as<bool>; }> {};

// clang-format on

template <template <typename...> typename, typename>
struct is_predicate_impl;

template <template <typename...> typename Fn, std::size_t... Is>
struct is_predicate_impl<Fn, std::index_sequence<Is...>> : std::disjunction<is_predicate_tester<Fn, replicate_t<void, Is + 1>>...> {};

} // namespace fmia::meta

export namespace fmia::meta {

template <template <typename...> typename Fn, std::size_t ArityLimit = 5>
  requires (ArityLimit > 0)
struct is_predicate : is_predicate_impl<Fn, std::make_index_sequence<ArityLimit>> {};

template <template <typename...> typename Fn, std::size_t ArityLimit = 5>
inline constexpr bool is_predicate_v = is_predicate<Fn, ArityLimit>::value;

template <template <typename...> typename Fn>
concept predicate = is_predicate_v<Fn>;

template <typename Fn>
concept wrapped_predicate = wrapped_template<Fn> && predicate<Fn::template type>;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename... AnyTypeLists>
struct concat_impl {
private:
  static constexpr bool is_indexed_ = is_indexed_type_list_v<AnyTypeLists...[0]>;

  // divide and conquer for >= 3 type lists for better time complexity:
  // 1. divide
  // `divide_helper_<BeginIdx, N / 2>` represents the left half
  // `divide_helper_<BeginIdx + N / 2, N - N / 2>` represents the right half
  // 2. merge
  // use the 2 lists specialization of `concat_impl` to merge
  //
  // placing this helper inside `concat_impl` improves compilation time significantly, since in this way we can avoid the unnecessary copies
  // of pack `Ts...` during the recursion
  template <std::size_t BeginIdx, std::size_t N>
  struct divide_helper_
    : concat_impl<typename divide_helper_<BeginIdx, N / 2>::type, typename divide_helper_<BeginIdx + N / 2, N - N / 2>::type> {};

  template <std::size_t BeginIdx>
  struct divide_helper_<BeginIdx, 1> {
    using type = AnyTypeLists...[BeginIdx];
  };

  template <std::size_t BeginIdx>
  struct divide_helper_<BeginIdx, 0> {
    using type = std::conditional_t<is_indexed_, empty_indexed_type_list, empty_type_list>;
  };

public:
  using type = divide_helper_<0, sizeof...(AnyTypeLists)>::type;
};

template <typename... Ts>
struct concat_impl<type_list<Ts...>> {
  using type = type_list<Ts...>;
};

template <std::size_t... Is, typename... Ts>
struct concat_impl<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>> {
  using type = indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>;
};

template <typename... Ts, typename... Us>
struct concat_impl<type_list<Ts...>, type_list<Us...>> {
  using type = type_list<Ts..., Us...>;
};

template <std::size_t... Is, typename... Ts, std::size_t... Js, typename... Us>
struct concat_impl<
  indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>, indexed_type_list<std::index_sequence<Js...>, type_list<Us...>>
> {
  using type = indexed_type_list<std::index_sequence<Is..., Js...>, type_list<Ts..., Us...>>;
};

template <typename Result, bool = is_indexed_type_list_v<Result>>
struct concat_get_result_helper;

template <typename Result>
struct concat_get_result_helper<Result, false> {
  using type = Result;
};

template <typename Result>
  requires is_no_duplication_integer_sequence_v<typename Result::indices>
struct concat_get_result_helper<Result, true> {
  using type = Result;
};

template <typename Result>
struct concat_get_result_helper<Result, true> {};

} // namespace fmia::meta

export namespace fmia::meta {

// name after Haskell Data.List concat
// get the concatenation of several type lists
// O(log n) time complexity for type_list, where n is the count of type lists to concatenate
// O(nlog n) time complexity for indexed_type_list (because of the validation for indices), where n is the sum of types over all given lists
template <list_of_types... Ts>
using concat = concat_get_result_helper<typename concat_impl<Ts...>::type>;

template <list_of_types... Ts>
using concat_t = concat<Ts...>::type;

// get a type list that contains types whose real indices are in the given `std::index_sequence`
// O(1) time complexity
template <typename, typename>
struct select_by_index_sequence;

template <typename... Ts, std::size_t... Is>
struct select_by_index_sequence<type_list<Ts...>, std::index_sequence<Is...>> {
  using type = type_list<Ts...[Is]...>;
};

template <std::size_t... Indices, typename... Ts, std::size_t... Is>
struct select_by_index_sequence<indexed_type_list<std::index_sequence<Indices...>, type_list<Ts...>>, std::index_sequence<Is...>> {
  using type = indexed_type_list<std::index_sequence<Indices...[Is]...>, type_list<Ts...[Is]...>>;
};

template <typename AnyTypeList, typename IndexSequence>
using select_by_index_sequence_t = select_by_index_sequence<AnyTypeList, IndexSequence>::type;

// name after Haskell Data.List reverse
// get a type list that is the reverse of the given list
// O(1) time complexity
template <list_of_types T>
using reverse = select_by_index_sequence<T, make_reversed_index_sequence<length_v<T>>>;

template <list_of_types T>
using reverse_t = reverse<T>::type;

// name after Haskell Data.List tail
// get a type list with the first type removed comparing to the given list
// O(1) time complexity
template <nonempty_list_of_types T>
using tail = select_by_index_sequence<T, make_index_sequence_of_range<1, length_v<T> - 1>>;

template <nonempty_list_of_types T>
using tail_t = tail<T>::type;

// name after Haskell Data.List init
// get a type list with the last type removed comparing to the given list
// O(1) time complexity
template <nonempty_list_of_types T>
using init = select_by_index_sequence<T, std::make_index_sequence<length_v<T> - 1>>;

template <nonempty_list_of_types T>
using init_t = init<T>::type;

// name after Haskell Data.List take
// get a type list that contains the first N types of the given list
// O(1) time complexity
template <list_of_types T, std::size_t N>
  requires (N <= length_v<T>)
using take = select_by_index_sequence<T, std::make_index_sequence<N>>;

template <list_of_types T, std::size_t N>
using take_t = take<T, N>::type;

// same as take, but take from the end
// O(1) time complexity
template <list_of_types T, std::size_t N>
  requires (N <= length_v<T>)
struct take_end : select_by_index_sequence<T, make_index_sequence_of_range<length_v<T> - N, length_v<T> - 1>> {};

template <typename T>
  requires is_type_list_v<T>
struct take_end<T, 0> {
  using type = empty_type_list;
};

template <typename T>
  requires is_indexed_type_list_v<T>
struct take_end<T, 0> {
  using type = empty_indexed_type_list;
};

template <list_of_types T, std::size_t N>
using take_end_t = take_end<T, N>::type;

// name after Haskell Data.List drop
// get a type list with the first N types removed comparing to the given list
// O(1) time complexity
template <list_of_types T, std::size_t N>
  requires (N <= length_v<T>)
using drop = take_end<T, length_v<T> - N>;

template <list_of_types T, std::size_t N>
using drop_t = drop<T, N>::type;

// same as drop, but drop from the end
// O(1) time complexity
template <list_of_types T, std::size_t N>
  requires (N <= length_v<T>)
using drop_end = take<T, length_v<T> - N>;

template <list_of_types T, std::size_t N>
using drop_end_t = drop_end<T, N>::type;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename, template <typename> typename, typename = empty_type_list>
struct take_while_impl;

template <template <typename> typename Fn, typename TypeListOfTaken>
struct take_while_impl<empty_type_list, Fn, TypeListOfTaken> {
  using type = TypeListOfTaken;
};

template <typename, template <typename> typename, typename>
struct take_while_impl_lazy_evaluation_helper;

template <typename T, typename... Ts, template <typename> typename Fn, typename... Taken>
struct take_while_impl_lazy_evaluation_helper<type_list<T, Ts...>, Fn, type_list<Taken...>> {
  // cannot use inheritance here, otherwise the evaluation is not lazy
  using type = take_while_impl<type_list<Ts...>, Fn, type_list<Taken..., T>>::type;
};

template <typename T, typename... Ts, template <typename> typename Fn, typename... Taken>
struct take_while_impl<type_list<T, Ts...>, Fn, type_list<Taken...>>
  : std::conditional_t<
      Fn<T>::value, take_while_impl_lazy_evaluation_helper<type_list<T, Ts...>, Fn, type_list<Taken...>>,
      std::type_identity<type_list<Taken...>>
    > {};

} // namespace fmia::meta

export namespace fmia::meta {

// name after Haskell Data.List takeWhile
// get the longest prefix type list whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest prefix
template <list_of_types T, template <typename> typename Fn>
  requires predicate<Fn>
using take_while = take_while_impl<T, Fn>;

template <list_of_types T, template <typename> typename Fn>
  requires predicate<Fn>
using take_while_t = take_while<T, Fn>::type;

// get the longest suffix type list whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest suffix
template <list_of_types T, template <typename> typename Fn>
  requires predicate<Fn>
struct take_while_end : reverse<take_while_t<reverse_t<T>, Fn>> {};

template <list_of_types T, template <typename> typename Fn>
  requires predicate<Fn>
using take_while_end_t = take_while_end<T, Fn>::type;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename, template <typename> typename>
struct drop_while_impl;

template <template <typename> typename Fn>
struct drop_while_impl<empty_type_list, Fn> {
  using type = empty_type_list;
};

template <typename, template <typename> typename>
struct drop_while_impl_lazy_evaluation_helper;

template <typename T, typename... Ts, template <typename> typename Fn>
struct drop_while_impl_lazy_evaluation_helper<type_list<T, Ts...>, Fn> {
  // cannot use inheritance here, otherwise the evaluation is not lazy
  using type = drop_while_impl<type_list<Ts...>, Fn>::type;
};

template <typename T, typename... Ts, template <typename> typename Fn>
struct drop_while_impl<type_list<T, Ts...>, Fn>
  : std::conditional_t<
      Fn<T>::value, drop_while_impl_lazy_evaluation_helper<type_list<T, Ts...>, Fn>, std::type_identity<type_list<T, Ts...>>
    > {};

} // namespace fmia::meta

export namespace fmia::meta {

// name after Haskell Data.List dropWhile
// get a type list with a longest prefix type list removed, whose types all satisfy a given predicate
// O(k) time complexity, where k is the longest dropped prefix
template <list_of_types T, template <typename> typename Fn>
  requires predicate<Fn>
using drop_while = drop_while_impl<T, Fn>;

template <list_of_types T, template <typename> typename Fn>
  requires predicate<Fn>
using drop_while_t = drop_while<T, Fn>::type;

// name after Haskell Data.List dropWhileEnd
// get a type list with a longest suffix type list removed, whose types all satisfy a given predicate
// O(k) time complexity, where k is the length of the longest dropped suffix
template <list_of_types T, template <typename> typename Fn>
  requires predicate<Fn>
struct drop_while_end : reverse<drop_while_t<reverse_t<T>, Fn>> {};

template <list_of_types T, template <typename> typename Fn>
  requires predicate<Fn>
using drop_while_end_t = drop_while_end<T, Fn>::type;

// name after Haskell Data.List filter
// get a type list that contains all the types that satisfy a given predicate
// O(log n) time complexity, limited by `concat`, where n is the length of the given list
template <typename, template <typename> typename Fn>
  requires predicate<Fn>
struct filter;

template <template <typename> typename Fn>
struct filter<empty_type_list, Fn> {
  using type = empty_type_list;
};

template <typename... Ts, template <typename> typename Fn>
struct filter<type_list<Ts...>, Fn> : concat<std::conditional_t<Fn<Ts>::value, type_list<Ts>, empty_type_list>...> {};

template <typename TypeList, template <typename> typename Fn>
using filter_t = filter<TypeList, Fn>::type;

// get a type list that only contains the given type T comparing to the given list
// O(log n) time complexity, where n is the length of the given list
template <list_of_types T, typename U>
using keep = filter<T, bind_front<std::is_same, U>::template type>;

template <list_of_types T, typename U>
using keep_t = keep<T, U>::type;

// get a type list that does not contain the given type T comparing to the given list
// O(log n) time complexity, where n is the length of the given list
template <list_of_types T, typename U>
using remove = filter<T, bind_front<not_same, U>::template type>;

template <list_of_types T, typename U>
using remove_t = remove<T, U>::type;

// apply the given template to every type of the given list
// O(1) time complexity
template <typename, template <typename> typename>
struct transform;

template <typename... Ts, template <typename> typename Fn>
struct transform<type_list<Ts...>, Fn> {
  using type = type_list<typename Fn<Ts>::type...>;
};

template <typename TypeList, template <typename> typename Fn>
using transform_t = transform<TypeList, Fn>::type;

// name after Haskell Data.List any
// test if any of the types of the given list satisfy the given predicate
// O(k) time complexity, where k is the length of the longest prefix whose types do not satisfy the given predicate
template <typename, template <typename> typename Fn>
  requires predicate<Fn>
struct any;

template <typename... Ts, template <typename> typename Fn>
struct any<type_list<Ts...>, Fn> : std::disjunction<Fn<Ts>...> {};

template <typename TypeList, template <typename> typename Fn>
  requires predicate<Fn>
inline constexpr bool any_v = any<TypeList, Fn>::value;

// test if none of the types of the given list satisfy the given predicate
template <typename TypeList, template <typename> typename Fn>
  requires predicate<Fn>
using none = std::negation<any<TypeList, Fn>>;

template <typename TypeList, template <typename> typename Fn>
  requires predicate<Fn>
inline constexpr bool none_v = none<TypeList, Fn>::value;

// name after Haskell Data.List all
// test if all of the types of the given list satisfy the given predicate
// O(k) time complexity, where k is the length of the longest prefix whose types satisfy the given predicate
template <typename, template <typename> typename Fn>
  requires predicate<Fn>
struct all;

template <template <typename> typename Fn>
struct all<empty_type_list, Fn> : std::false_type {};

template <typename... Ts, template <typename> typename Fn>
struct all<type_list<Ts...>, Fn> : std::conjunction<Fn<Ts>...> {};

template <typename TypeList, template <typename> typename Fn>
  requires predicate<Fn>
inline constexpr bool all_v = all<TypeList, Fn>::value;

template <typename, typename>
struct has_any;

template <typename T>
struct has_any<empty_type_list, T> : std::false_type {};

template <typename T>
struct has_any<empty_indexed_type_list, T> : std::false_type {};

template <typename... Ts, typename T>
struct has_any<type_list<Ts...>, T> : is_any_of<T, Ts...> {};

template <std::size_t... Is, typename... Ts, typename T>
struct has_any<indexed_type_list<std::index_sequence<Is...>, type_list<Ts...>>, T> : is_any_of<T, Ts...> {};

template <typename AnyTypeList, typename T>
inline constexpr bool has_any_v = has_any<AnyTypeList, T>::value;

template <list_of_types T, typename U>
using has_none = std::negation<has_any<T, U>>;

template <list_of_types T, typename U>
inline constexpr bool has_none_v = has_none<T, U>::value;

} // export namespace fmia::meta
