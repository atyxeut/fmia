// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.data_structure.string.pattern_matching;

import std;

import fmia.random;
import fmia.data_structure.string.pattern_matching;

using namespace fmia;

constexpr std::size_t test_count = 1e3;
constexpr std::size_t str_len = 1e3;

export {

void check_kmp_search() {
  for (auto i = 0uz; i < test_count; ++i) {
    const auto src_0 = std::string(str_len, 'a');
    const auto pattern_0 = std::string(random::rand(0uz, str_len), 'a');
    const auto next_0 = kmp_search_next_array(pattern_0);

    const auto src_1 = random::rand_ascii_string(str_len);
    const auto pattern_1 = random::rand_ascii_string(random::rand(0uz, str_len));
    const auto next_1 = kmp_search_next_array(pattern_1);

    { // check kmp_search_first
      const bool res_0 = src_0.find(pattern_0) == kmp_search_first(src_0, pattern_0, next_0);
      contract_assert(res_0);
      const bool res_1 = src_1.find(pattern_1) == kmp_search_first(src_1, pattern_1, next_1);
      contract_assert(res_1);
    }

    { // check kmp_search_all
      const auto res_std = [](std::string_view src, std::string_view pattern) -> std::vector<std::size_t> {
        if (pattern.empty())
          return {0};
        std::vector<std::size_t> res;
        for (auto pos = src.find(pattern); pos != std::string_view::npos; pos = src.find(pattern, pos + 1))
          res.push_back(pos);
        return res;
      };
      const bool res_0 = res_std(src_0, pattern_0) == kmp_search_all(src_0, pattern_0, next_0);
      contract_assert(res_0);
      const bool res_1 = res_std(src_1, pattern_1) == kmp_search_all(src_1, pattern_1, next_1);
      contract_assert(res_1);
    }
  }
}

} // export
