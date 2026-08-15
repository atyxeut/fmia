// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

module;

#include <macro/wconversion_flag.hpp>

export module fmia.math.integer.arbitrary_precision;

import std;

import fmia.data_structure.string.character;
import fmia.io.format_flag;
import fmia.math.integer.fixed_precision;

export namespace fmia::meta {

template <typename>
struct is_no_cv_arbitrary_precision_integral : std::false_type {};

template <typename T>
inline constexpr bool is_no_cv_arbitrary_precision_integral_v = is_no_cv_arbitrary_precision_integral<T>::value;

template <typename T>
using is_arbitrary_precision_integral = is_no_cv_arbitrary_precision_integral<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_arbitrary_precision_integral_v = is_arbitrary_precision_integral<T>::value;

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

// basic utils for unsigned operands in radix 10^k

template <std::ranges::bidirectional_range R>
void print(R&& number, iofmt format = iofmt::none) {
  for (const auto limb : number | std::views::reverse)
    std::print("{}", limb);
  if (format == iofmt::endl)
    std::println();
}

// s must be an unsigned integer, otherwise the behavior is undefined
template <std::integral Limb = int>
[[nodiscard]] constexpr std::vector<Limb> parse(std::string_view s) {
  const auto n = s.size();
  if (n == 1 && s[0] == '0')
    return {};

  std::vector<Limb> res(n);
  for (auto i = 0uz; i < n; ++i)
    res[i] = s[n - i - 1] - '0';
  return res;
}

template <std::integral Limb = int>
[[nodiscard]] std::vector<Limb> input() {
  std::string s;
  std::cin >> s;
  return parse<Limb>(s);
}

template <std::integral Limb>
[[nodiscard]] constexpr bool is_zero(const std::vector<Limb>& number) noexcept {
  return number.empty();
}

// -1: a < b, 0: a = b, 1: a > b
template <std::integral Limb>
[[nodiscard]] constexpr int compare(std::span<const Limb> a, std::span<const Limb> b) noexcept {
  const auto al = a.size();
  const auto bl = b.size();
  if (al != bl)
    return (al > bl) - (al < bl);
  for (auto i = al; i > 0;) {
    --i;
    if (a[i] != b[i])
      return (a[i] > b[i]) - (a[i] < b[i]);
  }
  return 0;
}

} // export namespace fmia::big_integer::naive

namespace fmia::big_integer::naive {

enum class carry_policy { none, assume_unsigned };

template <carry_policy Policy, typename Limb, Limb Radix, typename Range>
constexpr Limb carry_impl(Range&& number) noexcept {
  Limb carry = 0;
  for (auto& limb : number) {
    limb += carry;
    carry = limb / Radix;
    limb %= Radix;
    if (Policy == carry_policy::none && limb < 0) {
      --carry;
      limb += Radix;
    }
  }
  return carry;
}

} // namespace fmia::big_integer::naive

export namespace fmia::big_integer::naive {

template <std::integral Limb, Limb Radix = 10, std::ranges::forward_range R>
constexpr auto carry_unsigned(R&& number) noexcept {
  return carry_impl<carry_policy::assume_unsigned, Limb, Radix>(std::forward<R>(number));
}

template <std::integral Limb, Limb Radix = 10, std::ranges::forward_range R>
constexpr auto carry(R&& number) noexcept {
  return carry_impl<carry_policy::none, Limb, Radix>(std::forward<R>(number));
}

template <std::integral Limb>
constexpr void remove_lz(std::vector<Limb>& number) noexcept {
  while (!number.empty() && number.back() == 0)
    number.pop_back();
}

template <std::integral Limb>
[[nodiscard]] constexpr std::vector<Limb> add(const std::vector<Limb>& a, const std::vector<Limb>& b) {
  if (is_zero(a))
    return b;
  if (is_zero(b))
    return a;

  std::vector<Limb> ans(std::max(a.size(), b.size()) + 1);
  for (auto i = 0uz; i < a.size(); ++i)
    ans[i] += a[i];
  for (auto i = 0uz; i < b.size(); ++i)
    ans[i] += b[i];
  carry_unsigned<Limb>(ans);
  remove_lz(ans);
  return ans;
}

template <std::integral Limb>
struct sub_result {
  bool negative;
  std::vector<Limb> mag;
};

template <std::integral Limb>
void print(const sub_result<Limb>& result, iofmt format = iofmt::none) {
  if (result.negative)
    std::print("-");
  print(result.mag, format);
}

template <std::integral Limb>
[[nodiscard]] constexpr sub_result<Limb> sub(const std::vector<Limb>& a, const std::vector<Limb>& b) {
  if (is_zero(a))
    return {true, b};
  if (is_zero(b))
    return {false, a};

  std::vector<Limb> ans(std::max(a.size(), b.size()));
  const int sgn = compare<int>(a, b);
  for (auto i = 0uz; i < a.size(); ++i)
    ans[i] += a[i] * sgn;
  for (auto i = 0uz; i < b.size(); ++i)
    ans[i] -= b[i] * sgn;
  carry<Limb>(ans);
  remove_lz(ans);
  return {sgn < 0, std::move(ans)};
}

template <std::integral Limb>
[[nodiscard]] constexpr std::vector<Limb> mul(const std::vector<Limb>& a, const std::vector<Limb>& b) {
  if (is_zero(a) || is_zero(b))
    return std::vector<Limb> {};

  // delayed carry is always safe here, ans[k] accumulates at most min(la, lb) additions, assume that every addition is ans[k] += 9 * 9,
  // it requires (2^31 - 1) / 81 > 1e7 additions to overflow, such inputs are far beyond the capability of this O(n^2) algorithm
  std::vector<Limb> ans(a.size() + b.size());

  for (auto i = 0uz; i < a.size(); ++i)
    for (auto j = 0uz; j < b.size(); ++j)
      ans[i + j] += a[i] * b[j];

  carry_unsigned<Limb>(ans);
  remove_lz(ans);
  return ans;
}

template <std::integral Limb, typename Remainder>
struct floor_div_result {
  std::vector<Limb> q;
  Remainder r;
};

FMIA_WCONVERSION_PUSH()

// used when b is way smaller than a
template <std::integral Limb>
[[nodiscard]] constexpr floor_div_result<Limb, long long> floor_div(const std::vector<Limb>& a, int b)
// ICE in debug mode
// pre(b != 0)
{
  std::vector<Limb> q(a.size());
  long long r = 0;
  for (auto i = q.size(); i > 0;) {
    --i;
    r = r * 10 + a[i];
    q[i] = r / b;
    r %= b;
  }
  remove_lz(q);
  return {std::move(q), r};
}

template <std::integral Limb>
[[nodiscard]] constexpr floor_div_result<Limb, std::vector<Limb>> floor_div(const std::vector<Limb>& a, const std::vector<Limb>& b) pre(
  !is_zero(b)
) {
  const int comp_result = compare<Limb>(a, b);
  if (comp_result < 0)
    return {{}, a};
  if (comp_result == 0)
    return {{1}, {}};

  std::vector<Limb> q(a.size() - b.size() + 1);
  std::vector<Limb> r(a);
  bool not_first_digit = false;
  for (auto i = q.size(); i > 0;) {
    --i;
    while ((not_first_digit && r[i + b.size()] != 0) || compare<Limb>(std::span(r.begin() + i, b.size()), b) >= 0) {
      ++q[i];
      for (auto j = 0uz; j < b.size(); ++j)
        r[i + j] -= b[j];
      carry<Limb>(std::span(r.begin() + i, b.size() + not_first_digit));
    }
    not_first_digit = true;
  }
  remove_lz(q);
  r.resize(b.size());
  remove_lz(r);
  return {std::move(q), std::move(r)};
}

FMIA_WCONVERSION_POP()

template <std::integral Limb>
struct div_result {
  std::vector<Limb> q_int;
  std::vector<Limb> q_frac; // big endian
};

template <std::integral Limb>
void print(const div_result<Limb>& result, iofmt format = iofmt::none) {
  print(result.q_int);
  if (!result.q_frac.empty())
    std::print(".");
  print(std::views::reverse(result.q_frac), format);
}

FMIA_WCONVERSION_PUSH()

// will calculate to precision + 1 decimal digits and round to the nearest
template <std::integral Limb>
[[nodiscard]] constexpr div_result<Limb> div(const std::vector<Limb>& a, int b, usize precision = 16) {
  auto [q_int, r] = floor_div(a, b);
  if (precision == 0) {
    if (r * 10 / b >= 5) {
      ++q_int[0];
      if (Limb carry = carry_unsigned<Limb>(q_int); carry > 0)
        q_int.resize(q_int.size() + 1, carry);
    }
    return {std::move(q_int), std::vector<Limb> {}};
  }

  ++precision;
  std::vector<Limb> q_frac(precision);
  for (auto i = 0uz; i < precision; ++i) {
    r *= 10;
    q_frac[i] = r / b;
    r %= b;
  }
  if (q_frac.back() >= 5) {
    ++q_frac[precision - 2];
    if (carry_unsigned<int>(std::views::reverse(std::span(q_frac.begin(), q_frac.begin() + precision - 1))) > 0) {
      ++q_int[0];
      if (Limb carry = carry_unsigned<Limb>(q_int); carry != 0)
        q_int.resize(q_int.size() + 1, carry);
    }
  }

  q_frac.pop_back();
  return {std::move(q_int), std::move(q_frac)};
}

FMIA_WCONVERSION_POP()

template <std::integral Limb>
[[nodiscard]] constexpr std::vector<Limb> pow(std::vector<Limb> a, int n) pre(n >= 0) {
  if (n == 0)
    return {1};

  std::vector<Limb> ans {1};
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

[[nodiscard]] constexpr std::vector<int> floor_root(const std::vector<int>& a, int n) pre(n > 0) {
  if (n == 1 || is_zero(a))
    return a;

  std::vector<int> l;
  std::vector<int> r(pow(std::vector<int> {0, 1}, a.size() / n + 1));
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

[[nodiscard]] constexpr auto floor_sqrt(const std::vector<int>& a) {
  return floor_root(a, 2);
}

[[nodiscard]] constexpr auto floor_cbrt(const std::vector<int>& a) {
  return floor_root(a, 3);
}

} // export namespace fmia::big_integer::naive

namespace fmia::big_integer {

[[nodiscard]] constexpr auto preprocess_input_string(std::string_view s) {
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
[[nodiscard]] constexpr auto get_digit_limb_count(std::string_view s_after_preprocess) {
  return (s_after_preprocess.size() + DigitsPerLimb - 1) / DigitsPerLimb;
}

template <typename Limb, Limb DigitsPerLimb>
constexpr void set_digit_limbs(std::string_view s_after_preprocess, std::span<Limb> dest) {
  for (const char *l, *r = s_after_preprocess.data() + s_after_preprocess.size(); auto& limb : dest) {
    l = std::max(r - DigitsPerLimb, s_after_preprocess.data());
    if (const auto res = std::from_chars(l, r, limb); res.ptr != r)
      throw std::invalid_argument("invalid integer: not a number");
    r = l;
  }
}

template <typename Limb, Limb DigitsPerLimb>
void print_digit_limbs(std::ostream& ostr, std::span<const Limb> num) {
  auto i = num.size();
  ostr << num[--i]; // the first limb doesn't need padding zeros

  ostr << std::setfill('0');
  while (i > 0)
    ostr << std::setw(DigitsPerLimb) << num[--i];
  ostr << std::setfill(' ');
}

} // namespace fmia::big_integer

export namespace fmia::big_integer::naive {

class digit_storage {
public:
  using limb_type = i64;

  // (2^63 - 1) / (1e6 * 1e6) > 9e6
  static constexpr limb_type radix = 1e6;
  static constexpr limb_type digits_per_limb = 6;

private:
  bool negative_;
  std::vector<limb_type> mag_;

public:
  constexpr digit_storage() noexcept = default;

  explicit constexpr digit_storage(std::string_view s) {
    const auto ns = preprocess_input_string(s);
    negative_ = s[0] == '-';
    mag_.resize(get_digit_limb_count<limb_type, digits_per_limb>(ns));
    set_digit_limbs<limb_type, digits_per_limb>(ns, mag_);
  }

  friend auto& operator >>(std::istream& istr, digit_storage& n) {
    std::string s;
    istr >> s;
    n = digit_storage(s);
    return istr;
  }

  friend auto& operator <<(std::ostream& ostr, const digit_storage& n) {
    if (n.negative_)
      ostr << '-';
    print_digit_limbs<limb_type, digits_per_limb>(ostr, n.mag_);
    return ostr;
  }
};

} // export namespace fmia::big_integer::naive

export namespace fmia::meta {

template <>
struct is_no_cv_arbitrary_precision_integral<big_integer::naive::digit_storage> : std::true_type {};

} // export namespace fmia::meta
