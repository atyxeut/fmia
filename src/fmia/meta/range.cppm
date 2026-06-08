// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.meta.range;

import std;

export namespace fmia::meta {

template <typename T, typename U>
concept range_of = std::ranges::range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept input_range_of = std::ranges::input_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept forward_range_of = std::ranges::forward_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept bidirectional_range_of = std::ranges::bidirectional_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept random_access_range_of = std::ranges::random_access_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

template <typename T, typename U>
concept contiguous_range_of = std::ranges::contiguous_range<T> && std::same_as<U, std::ranges::range_value_t<T>>;

} // export namespace fmia::meta
