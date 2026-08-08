// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.memory.iterator;

import std;

import fmia.meta;

export namespace fmia {

// use when there is a high probability that p, q are equal
template <std::input_iterator I, std::strict_weak_order<std::iter_reference_t<I>, std::iter_reference_t<I>> Cmp = std::ranges::less>
[[nodiscard]] constexpr bool iter_compare(I p, I q, Cmp cmp = std::ranges::less {}) noexcept(noexcept(cmp(*p, *q))) {
  if (p == q)
    return false;
  return cmp(*p, *q);
}

} // export namespace fmia
