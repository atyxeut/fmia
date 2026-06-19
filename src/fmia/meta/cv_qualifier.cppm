// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.meta.cv_qualifier;

import std;

export namespace fmia::meta {

template <typename T>
struct has_cv : std::bool_constant<std::is_const_v<T> || std::is_volatile_v<T>> {};

template <typename T>
inline constexpr bool has_cv_v = has_cv<T>::value;

template <typename T>
struct is_cv : std::bool_constant<std::is_const_v<T> && std::is_volatile_v<T>> {};

template <typename T>
inline constexpr bool is_cv_v = is_cv<T>::value;

} // export namespace fmia::meta

namespace fmia::meta {

template <typename From, typename To, bool = std::is_const_v<From>, bool = std::is_volatile_v<From>>
struct claim_cv_selector;

// branch 1: has both cv qualifiers
template <typename From, typename To>
struct claim_cv_selector<From, To, true, true> : std::add_cv<To> {};

// branch 2: has only const qualifier
template <typename From, typename To>
struct claim_cv_selector<From, To, true, false> : std::add_const<To> {};

// branch 3: has only volatile qualifier
template <typename From, typename To>
struct claim_cv_selector<From, To, false, true> : std::add_volatile<To> {};

// branch 4: has no cv qualifiers
template <typename From, typename To>
struct claim_cv_selector<From, To, false, false> {
  using type = To;
};

} // namespace fmia::meta

export namespace fmia::meta {

// extract the cv-qualifiers of From and apply them to To
template <typename From, typename To>
using claim_cv = claim_cv_selector<From, To>;

template <typename From, typename To>
using claim_cv_t = claim_cv<From, To>::type;

} // export namespace fmia::meta
