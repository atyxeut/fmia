// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.util.enum_flag;

import std;

export namespace fmia {

using enum_underlying_type = std::uint8_t;
using enum_flag_underlying_type = std::uint32_t;

} // export namespace fmia

export namespace fmia::meta {

template <typename>
struct is_enum_flag : std::false_type {};

template <typename T>
inline constexpr bool is_enum_flag_v = is_enum_flag<T>::value;

template <typename T>
concept enum_flag = is_enum_flag_v<T>;

} // export namespace fmia::meta

export namespace fmia::inline operator_overload::inline enum_flag_operator {

template <::fmia::meta::enum_flag T>
[[nodiscard]] constexpr auto operator &(T a, T b) noexcept {
  return static_cast<T>(std::to_underlying(a) & std::to_underlying(b));
};

template <::fmia::meta::enum_flag T>
constexpr auto& operator &=(T& a, T b) noexcept {
  return a = a & b;
};

template <::fmia::meta::enum_flag T>
[[nodiscard]] constexpr auto operator ^(T a, T b) noexcept {
  return static_cast<T>(std::to_underlying(a) ^ std::to_underlying(b));
};

template <::fmia::meta::enum_flag T>
constexpr auto& operator ^=(T& a, T b) noexcept {
  return a = a ^ b;
};

template <::fmia::meta::enum_flag T>
[[nodiscard]] constexpr auto operator |(T a, T b) noexcept {
  return static_cast<T>(std::to_underlying(a) | std::to_underlying(b));
};

template <::fmia::meta::enum_flag T>
constexpr auto& operator |=(T& a, T b) noexcept {
  return a = a | b;
};

} // export namespace fmia::inline operator_overload::inline enum_flag_operator
