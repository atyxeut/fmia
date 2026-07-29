// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.util.extrema;

import std;

export namespace fmia {

template <typename T, typename U, std::regular_invocable<T&, U&&> Cmp>
  requires std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>
[[nodiscard]] constexpr bool assign_if(T& dest, U&& src, Cmp fn) noexcept(
  noexcept(fn(dest, src)) && noexcept(dest = std::forward<U>(src))
) {
  if (fn(dest, src)) {
    dest = std::forward<U>(src);
    return true;
  }
  return false;
}

template <typename T, typename U>
  requires std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>
[[nodiscard]] constexpr bool update_max(T& dest, U&& src) noexcept(noexcept(assign_if(dest, std::forward<U>(src), std::ranges::less {}))) {
  return assign_if(dest, std::forward<U>(src), std::ranges::less {});
}

template <typename T, typename U>
  requires std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>
[[nodiscard]] constexpr bool update_min(T& dest, U&& src) noexcept(
  noexcept(assign_if(dest, std::forward<U>(src), std::ranges::greater {}))
) {
  return assign_if(dest, std::forward<U>(src), std::ranges::greater {});
}

} // export namespace fmia
