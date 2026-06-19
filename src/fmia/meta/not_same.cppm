// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.meta.not_same;

import std;

export namespace fmia::meta {

template <typename T, typename U>
struct not_same : std::negation<std::is_same<T, U>> {};

template <typename T, typename U>
inline constexpr bool not_same_v = not_same<T, U>::value;

template <typename T, typename U>
concept not_same_as = !std::same_as<T, U>;

template <typename T, typename U>
concept no_cvref_same_as = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template <typename T, typename U>
concept no_cvref_not_same_as = !no_cvref_same_as<T, U>;

} // export namespace fmia::meta
