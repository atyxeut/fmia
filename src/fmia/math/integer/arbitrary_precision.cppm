// Copyright (C) 2026 atyxeut
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <https://www.gnu.org/licenses/>.

module;

#include <cassert>

#include <macro/wconversion_switch.hpp>

export module fmia.math.integer.arbitrary_precision;

import std;

import fmia.data_structure.string.character.ascii;
import fmia.math.integer.fixed_precision;

export namespace fmia::meta {

template <typename>
struct is_no_cv_arbitrary_precision_integral : std::false_type
{
};

template <typename T>
constexpr bool is_no_cv_arbitrary_precision_integral_v = is_no_cv_arbitrary_precision_integral<T>::value;

template <typename T>
using is_arbitrary_precision_integral = is_no_cv_arbitrary_precision_integral<std::remove_cv_t<T>>;

template <typename T>
constexpr bool is_arbitrary_precision_integral_v = is_arbitrary_precision_integral<T>::value;

template <typename T>
concept arbitrary_precision_integral = is_arbitrary_precision_integral_v<T>;

template <typename T>
concept signed_integral = twos_complement_signed_integral<T> || arbitrary_precision_integral<T>;

template <typename T>
concept integral = fixed_precision_integral<T> || arbitrary_precision_integral<T>;

template <typename T>
concept nonbool_integral = integral<T> && !boolean<T>;

} // export namespace fmia::meta

export namespace fmia::big_integer::naive {

// +, -, *, /, %, power, root operations for nonnegative operands (radix 10^k) with free functions

[[nodiscard]] constexpr std::vector<int> parse(std::string_view s)
{
  const auto n = s.size();
  std::vector<int> ret(n);

  for (auto i = 0uz; i < n; ++i)
    ret[i] = s[n - i - 1] - '0';

  return ret;
}

[[nodiscard]] auto input()
{
  std::string s;
  std::cin >> s;

  return parse(s);
}

template <std::ranges::input_range R>
void print(R&& num, bool new_line = false)
{
  for (const auto digit : num | std::views::reverse)
    std::cout << digit;
  if (new_line)
    std::cout << '\n';
}

template <typename Limb>
[[nodiscard]] constexpr bool is_zero(const std::vector<Limb>& num) noexcept
{
  return num.size() == 1 && num[0] == 0;
}

// a < b : -1, a = b: 0, a > b: 1
template <typename Limb>
[[nodiscard]] constexpr int compare(std::span<const Limb> a, std::span<const Limb> b) noexcept
{
  const auto la = a.size(), lb = b.size();
  if (la == lb) {
    for (auto i = la; i > 0;) {
      --i;
      if (a[i] != b[i])
        return (a[i] > b[i]) - (a[i] < b[i]);
    }
  }
  return (la > lb) - (la < lb);
}

template <typename Limb, Limb Radix = 10, std::ranges::forward_range R>
constexpr auto carry_positive(R&& num) noexcept
{
  Limb c = 0;
  for (auto& limb : num) {
    limb += c;
    c = limb / Radix;
    limb %= Radix;
  }

  return c;
}

template <typename Limb, Limb Radix = 10, std::ranges::forward_range R>
constexpr auto carry(R&& num) noexcept
{
  Limb c = 0;
  for (Limb r; auto& limb : num) {
    limb += c;
    r = limb % Radix;
    c = limb / Radix - (r < 0);
    limb = r + Radix * (r < 0);
  }

  return c;
}

template <typename Limb>
constexpr void remove_lz(std::vector<Limb>& num) noexcept
{
  while (num.size() > 1 && num.back() == 0)
    num.pop_back();
}

[[nodiscard]] constexpr std::vector<int> add(const std::vector<int>& a, const std::vector<int>& b)
{
  std::vector<int> ans(std::max(a.size(), b.size()) + 1);

  for (auto i = 0uz; i < a.size(); ++i)
    ans[i] += a[i];
  for (auto i = 0uz; i < b.size(); ++i)
    ans[i] += b[i];

  carry_positive<int>(ans);
  remove_lz(ans);
  return ans;
}

struct sub_result
{
  int sgn;
  std::vector<int> mag;
};

void print(const sub_result& result, bool new_line = false)
{
  if (result.sgn < 0)
    std::cout << '-';
  print(result.mag, new_line);
}

[[nodiscard]] constexpr sub_result sub(const std::vector<int>& a, const std::vector<int>& b)
{
  std::vector<int> ans(std::max(a.size(), b.size()));

  const int sgn = compare<int>(a, b);

  for (auto i = 0uz; i < a.size(); ++i)
    ans[i] += a[i] * sgn;
  for (auto i = 0uz; i < b.size(); ++i)
    ans[i] -= b[i] * sgn;

  carry<int>(ans);
  remove_lz(ans);
  return {sgn, std::move(ans)};
}

[[nodiscard]] constexpr std::vector<int> mul(const std::vector<int>& a, const std::vector<int>& b)
{
  if (is_zero(a) || is_zero(b))
    return std::vector<int> {0};

  // delayed carry is always safe here, ans[k] accumulates at most min(la, lb) additions, assume that every addition is
  // ans[k] += 9 * 9, it still requires (2^31 - 1) / 81 > 1e7 additions to overflow, in such cases, the inputs are far
  // beyond the capability of this O(n^2) algorithm
  std::vector<int> ans(a.size() + b.size());

  for (auto i = 0uz; i < a.size(); ++i)
    for (auto j = 0uz; j < b.size(); ++j)
      ans[i + j] += a[i] * b[j];

  carry_positive<int>(ans);
  remove_lz(ans);
  return ans;
}

template <typename Remainder>
struct floor_div_result
{
  std::vector<int> q;
  Remainder r;
};

FMIA_WCONVERSION_PUSH()

// used when b is way smaller than a
[[nodiscard]] constexpr floor_div_result<int> floor_div(const std::vector<int>& a, int b)
{
  assert(b != 0);

  std::vector<int> q(a.size());
  long long r = 0;

  for (auto i = q.size(); i > 0;) {
    --i;
    r = r * 10 + a[i];
    q[i] = r / b;
    r %= b;
  }

  remove_lz(q);
  return {std::move(q), static_cast<int>(r)};
}

[[nodiscard]] constexpr floor_div_result<std::vector<int>>
floor_div(const std::vector<int>& a, const std::vector<int>& b)
{
  assert(!is_zero(b));

  const int cmp_result = compare<int>(a, b);
  if (cmp_result < 0)
    return {{0}, a};
  if (cmp_result == 0)
    return {{1}, {0}};

  std::vector<int> q(a.size() - b.size() + 1), r(a);

  bool not_first_digit = false;
  for (auto i = q.size(); i > 0;) {
    --i;
    while ((not_first_digit && r[i + b.size()] != 0) || compare<int>(std::span(r.begin() + i, b.size()), b) >= 0) {
      ++q[i];
      for (auto j = 0uz; j < b.size(); ++j)
        r[i + j] -= b[j];
      carry<int>(std::span(r.begin() + i, b.size() + not_first_digit));
    }
    not_first_digit = true;
  }

  remove_lz(q);
  r.resize(b.size());
  remove_lz(r);
  return {std::move(q), std::move(r)};
}

FMIA_WCONVERSION_POP()

struct div_result
{
  std::vector<int> q_int;
  std::vector<int> q_frac; // not store the digits in reverse order
};

void print(const div_result& result, bool new_line = false)
{
  print(result.q_int, false);
  if (!result.q_frac.empty())
    std::cout << '.';
  print(std::views::reverse(result.q_frac), new_line);
}

FMIA_WCONVERSION_PUSH()

// will calculate to precision + 1 decimal digits and round to the nearest
[[nodiscard]] constexpr div_result div(const std::vector<int>& a, int b, usize precision = 16)
{
  auto [q_int, r_] = floor_div(a, b);
  long long r = r_;

  if (precision == 0) {
    if (r * 10 / b >= 5) {
      ++q_int[0];
      if (int carry = carry_positive<int>(q_int); carry > 0)
        q_int.resize(q_int.size() + 1, carry);
    }
    return {std::move(q_int), std::vector<int> {}};
  }

  ++precision;
  std::vector<int> q_frac(precision);

  for (auto i = 0uz; i < precision; ++i) {
    r *= 10;
    q_frac[i] = r / b;
    r %= b;
  }
  if (q_frac.back() >= 5) {
    ++q_frac[precision - 2];
    if (carry_positive<int>(std::views::reverse(std::span(q_frac.begin(), q_frac.begin() + precision - 1))) > 0) {
      ++q_int[0];
      if (int carry = carry_positive<int>(q_int); carry != 0)
        q_int.resize(q_int.size() + 1, carry);
    }
  }

  q_frac.pop_back();
  return {std::move(q_int), std::move(q_frac)};
}

FMIA_WCONVERSION_POP()

[[nodiscard]] constexpr std::vector<int> pow(std::vector<int> a, int n)
{
  assert(n >= 0);

  if (n == 0)
    return {1};

  std::vector<int> ans {1};
  while (true) {
    if (n & 1)
      ans = mul(ans, a);
    n >>= 1;
    if (n == 0)
      break;
    a = mul(a, a);
  }
  return ans;
}

FMIA_WCONVERSION_PUSH()

[[nodiscard]] constexpr std::vector<int> floor_root(const std::vector<int>& a, int n)
{
  assert(n > 0);

  if (n == 1 || is_zero(a))
    return a;

  std::vector<int> l {0}, r(pow(std::vector<int> {0, 1}, a.size() / n + 1));
  while (compare<int>(add(l, std::vector<int> {1}), r) < 0) {
    std::vector<int> mid(floor_div(add(l, r), 2).q);
    if (compare<int>(pow(mid, n), a) <= 0)
      l = std::move(mid);
    else
      r = std::move(mid);
  }
  return l;
}

FMIA_WCONVERSION_POP()

[[nodiscard]] constexpr auto floor_sqrt(const std::vector<int>& a)
{
  return floor_root(a, 2);
}

[[nodiscard]] constexpr auto floor_cbrt(const std::vector<int>& a)
{
  return floor_root(a, 3);
}

} // export namespace fmia::big_integer::naive

namespace fmia::big_integer {

[[nodiscard]] constexpr auto preprocess_input_string(std::string_view s)
{
  if (s.empty() || (s.size() == 1) & !is_ascii_digit(s[0]))
    throw std::invalid_argument("invalid integer: no digits");

  if (s.size() > 1 & s[0] != '-' & !is_ascii_digit(s[0]))
    throw std::invalid_argument("invalid integer: not a number");

  auto i = 0uz;
  for (; i + 1 < s.size(); ++i)
    if ('1' <= s[i] & s[i] <= '9')
      break;
  return s.substr(i);
}

template <typename Limb, Limb DigitsPerLimb>
[[nodiscard]] constexpr auto get_digit_limb_count(std::string_view s_after_preprocess)
{
  return (s_after_preprocess.size() + DigitsPerLimb - 1) / DigitsPerLimb;
}

template <typename Limb, Limb DigitsPerLimb>
constexpr void set_digit_limbs(std::string_view s_after_preprocess, std::span<Limb> dest)
{
  for (const char *l, *r = s_after_preprocess.data() + s_after_preprocess.size(); auto& limb : dest) {
    l = std::max(r - DigitsPerLimb, s_after_preprocess.data());
    if (const auto res = std::from_chars(l, r, limb); res.ptr != r)
      throw std::invalid_argument("invalid integer: not a number");
    r = l;
  }
}

template <typename Limb, Limb DigitsPerLimb>
void print_digit_limbs(std::ostream& ostr, std::span<const Limb> num)
{
  auto i = num.size();
  ostr << num[--i]; // the first limb doesn't need padding zeros

  ostr << std::setfill('0');
  while (i > 0)
    ostr << std::setw(DigitsPerLimb) << num[--i];
  ostr << std::setfill(' ');
}

} // namespace fmia::big_integer

export namespace fmia::big_integer::naive {

class digit_storage
{
public:
  using limb_type = i64;

  // (2^63 - 1) / (1e6 * 1e6) > 9e6
  static constexpr limb_type radix = 1e6;
  static constexpr limb_type digits_per_limb = 6;

private:
  limb_type sgn_;
  std::vector<limb_type> mag_;

public:
  constexpr digit_storage() noexcept = default;

  explicit constexpr digit_storage(std::string_view s)
  {
    const auto ns = preprocess_input_string(s);
    sgn_ = s[0] == '-' ? -1 : 1;
    mag_.resize(get_digit_limb_count<limb_type, digits_per_limb>(ns));
    set_digit_limbs<limb_type, digits_per_limb>(ns, mag_);
  }

  friend auto& operator >>(std::istream& istr, digit_storage& n)
  {
    std::string s;
    istr >> s;
    n = digit_storage(s);
    return istr;
  }

  friend auto& operator <<(std::ostream& ostr, const digit_storage& n)
  {
    if (n.sgn_ < 0)
      ostr << '-';
    print_digit_limbs<limb_type, digits_per_limb>(ostr, n.mag_);
    return ostr;
  }
};

} // export namespace fmia::big_integer::naive

export namespace fmia::meta {

template <>
struct is_no_cv_arbitrary_precision_integral<big_integer::naive::digit_storage> : std::true_type
{
};

} // export namespace fmia::meta
