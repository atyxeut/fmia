// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.util;

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

// same values map to the same rank
template <std::ranges::forward_range Range>
[[nodiscard]] constexpr auto get_compressed_indices_ordered(const Range& src) -> std::vector<int> {
  std::vector<std::ranges::range_value_t<Range>> tmp(std::ranges::begin(src), std::ranges::end(src));
  std::sort(tmp.begin(), tmp.end());
  const auto tmp_end = std::unique(tmp.begin(), tmp.end());

  const auto n = std::ranges::size(src);
  std::vector<int> rank(n);
  const auto tmp_begin = tmp.begin();
  for (auto i = 0uz; const auto& elem : src)
    rank[i++] = static_cast<int>(std::lower_bound(tmp_begin, tmp_end, elem) - tmp_begin);

  return rank;
}

// every value maps to a unique rank, the value that has smaller index in the original range maps to a lower rank
template <std::ranges::input_range Range>
[[nodiscard]] constexpr auto get_compressed_indices_ordered_unique(Range&& src) -> std::vector<int> {
  const auto n = std::ranges::size(src);
  std::vector<std::pair<std::ranges::range_value_t<Range>, std::size_t>> tmp;
  tmp.reserve(n);
  for (auto i = 0uz; auto&& elem : src) {
    if constexpr (std::is_rvalue_reference_v<Range>)
      tmp.emplace_back(std::move(elem), i++);
    else
      tmp.emplace_back(elem, i++);
  }
  std::sort(tmp.begin(), tmp.end());

  std::vector<int> rank(n);
  for (auto i = 0uz; i < n; ++i)
    rank[tmp[i].second] = static_cast<int>(i);

  return rank;
}

} // export namespace fmia
