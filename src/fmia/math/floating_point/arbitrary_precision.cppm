// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.math.floating_point.arbitrary_precision;

import std;

import fmia.math.floating_point.ieee754;

export namespace fmia::meta {

template <typename>
struct is_no_cv_arbitrary_precision_floating_point : std::false_type
{
};

template <typename T>
inline constexpr bool is_no_cv_arbitrary_precision_floating_point_v = is_no_cv_arbitrary_precision_floating_point<T>::value;

template <typename T>
using is_arbitrary_precision_floating_point = is_no_cv_arbitrary_precision_floating_point<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_arbitrary_precision_floating_point_v = is_arbitrary_precision_floating_point<T>::value;

template <typename T>
concept arbitrary_precision_floating_point = is_arbitrary_precision_floating_point_v<T>;

template <typename T>
concept floating_point = ieee754_floating_point<T> || arbitrary_precision_floating_point<T>;

} // export namespace fmia::meta
