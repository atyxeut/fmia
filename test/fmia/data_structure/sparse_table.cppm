// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.data_structure.sparse_table;

import std;

import fmia.data_structure.sparse_table;
import fmia.math.algebra;
import fmia.math.integer.fixed_precision;
import fmia.random;

using namespace fmia;

constexpr usize element_count = 1e6;
constexpr usize max_jump_count = ilog2(element_count);

[[nodiscard]] std::vector<int> make_array() {
  std::vector<int> ret(element_count);
  for (auto rand_elem = random::uniform_distribution(0, std::numeric_limits<int>::max()); int& elem : ret)
    elem = rand_elem(random::mt19937_engine);
  return ret;
}

constexpr usize query_count = 1e6;

[[nodiscard]] std::vector<std::array<usize, 2>> make_query() {
  std::vector<std::array<usize, 2>> ret(query_count);
  for (auto rand_l = random::uniform_distribution(0uz, element_count - 1); auto& [l, r] : ret) {
    l = rand_l(random::mt19937_engine);
    r = random::rand(l, element_count - 1);
  }
  return ret;
}

template <typename F>
class naive_sparse_table final {
private:
  std::vector<std::vector<int>> st_;
  F f_;

public:
  naive_sparse_table(const std::vector<int>& data, const F& f) : st_(max_jump_count + 1, std::vector<int>(element_count)), f_(f) {
    std::copy(data.begin(), data.end(), st_[0].begin());
    for (usize j = 1; j <= max_jump_count; ++j)
      for (usize i = 0; i + (1 << j) <= element_count; ++i)
        st_[j][i] = f_(st_[j - 1][i], st_[j - 1][i + (1 << j - 1)]);
  }

  [[nodiscard]] int query_idempotent(usize l, usize r) const noexcept {
    const int j = ilog2(r - l + 1);
    return f_(st_[j][l], st_[j][r + 1 - (1 << j)]);
  }

  [[nodiscard]] int query_non_idempotent(usize l, usize r) const noexcept {
    int ans = 1;
    const usize d = r - l + 1;
    for (usize j = ilog2(d); l <= r; --j) {
      if (d >> j & 1) {
        ans = f_(ans, st_[j][l]);
        l += 1 << j;
      }
    }
    return ans;
  }
};

template <typename F>
naive_sparse_table(const std::vector<int>&, const F&) -> naive_sparse_table<F>;

export {

void check_idempotent_query() {
  auto a = make_array();
  auto f = idempotent_operator_wrapper(std::greater<int> {});

  naive_sparse_table naive_st(a, f);
  sparse_table st(a, f);

  auto q = make_query();
  for (const auto [l, r] : q) {
    if (const int naive_st_result = naive_st.query_idempotent(l, r), st_result = st.query(l, r); naive_st_result != st_result) {
      std::println(std::cerr, "differ in idempotent query\nnaive impl result: {}\nlib result: {}", naive_st_result, st_result);
      return;
    }
  }
  std::println(std::cerr, "idempotent query OK");
}

void check_non_idempotent_query() {
  constexpr int mod = 100019;

  auto a = make_array();
  auto f = [mod](int a, int b) {
    return 1ll * a * b % mod;
  };

  naive_sparse_table naive_st(a, f);
  // note that the sparse_table doesn't modify the first layer inside, this is for less overhead, since rarely does it need a modification
  sparse_table st(a | std::views::transform([mod](int x) { return x % mod; }), f);

  auto q = make_query();
  for (const auto [l, r] : q) {
    if (const int naive_st_result = naive_st.query_non_idempotent(l, r), st_result = st.query(l, r); naive_st_result != st_result) {
      std::println(std::cerr, "differ in non-idempotent query\nnaive impl result: {}\nlib result: {}", naive_st_result, st_result);
      return;
    }
  }
  std::println(std::cerr, "non-idempotent query OK");
}

} // export
