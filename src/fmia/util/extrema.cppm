// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.util.extrema;

import std;

export namespace fmia {

template <typename T, std::regular_invocable<const T&, const T&> Cmp>
[[nodiscard]] bool assign_if(T& dest, T&& src, Cmp fn) noexcept(noexcept(fn(dest, src)) && noexcept(dest = std::forward<T>(src))) {
  if (fn(dest, src)) {
    dest = std::forward<T>(src);
    return true;
  }
  return false;
}

template <typename T>
[[nodiscard]] bool update_max(T& dest, T&& src) noexcept(noexcept(assign_if(dest, std::forward<T>(src), std::ranges::less {}))) {
  return assign_if(dest, std::forward<T>(src), std::ranges::less {});
}

template <typename T>
[[nodiscard]] bool update_min(T& dest, T&& src) noexcept(noexcept(assign_if(dest, std::forward<T>(src), std::ranges::greater {}))) {
  return assign_if(dest, std::forward<T>(src), std::ranges::greater {});
}

} // export namespace fmia
