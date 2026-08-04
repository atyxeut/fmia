// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.string.pattern_matching;

import std;

export namespace fmia {

[[nodiscard]] constexpr std::vector<std::size_t> kmp_search_next_array(std::string_view pattern) noexcept {
  std::vector<std::size_t> res(pattern.size());
  for (auto i = 1uz, len = 0uz; i < pattern.size();) {
    if (pattern[i] == pattern[len])
      res[i++] = ++len;
    else if (len != 0)
      len = res[len - 1];
    else
      ++i;
  }
  return res;
}

} // export namespace fmia

namespace fmia {

enum class kmp_search_policy { first, all };

template <kmp_search_policy P>
using kmp_search_result_t = std::conditional_t<P == kmp_search_policy::first, std::size_t, std::vector<std::size_t>>;

template <kmp_search_policy P, typename R>
[[nodiscard]] constexpr kmp_search_result_t<P> kmp_search_impl(std::string_view src, std::string_view pattern, R&& next) noexcept {
  if (pattern.empty()) {
    if constexpr (P == kmp_search_policy::first)
      return 0;
    else
      return {0};
  }

  if (src.size() < pattern.size()) {
    if constexpr (P == kmp_search_policy::first)
      return std::string_view::npos;
    else
      return {};
  }

  kmp_search_result_t<P> res [[maybe_unused]];
  for (auto i = 0uz, j = 0uz; i < src.size();) {
    if (src[i] == pattern[j]) {
      ++i;
      ++j;
      if (j == pattern.size()) {
        if constexpr (P == kmp_search_policy::first) {
          return i - j;
        } else {
          res.push_back(i - j);
          j = next[j - 1];
        }
      }
    } else if (j == 0) {
      ++i;
    } else {
      j = next[j - 1];
    }
  }

  if constexpr (P == kmp_search_policy::first)
    return std::string_view::npos;
  else
    return res;
}

} // namespace fmia

export namespace fmia {

template <std::ranges::random_access_range R>
[[nodiscard]] constexpr auto kmp_search_first(std::string_view src, std::string_view pattern, R&& next) noexcept {
  return kmp_search_impl<kmp_search_policy::first>(src, pattern, std::forward<R>(next));
}

template <std::ranges::random_access_range R>
[[nodiscard]] constexpr auto kmp_search_all(std::string_view src, std::string_view pattern, R&& next) noexcept {
  return kmp_search_impl<kmp_search_policy::all>(src, pattern, std::forward<R>(next));
}

} // export namespace fmia
