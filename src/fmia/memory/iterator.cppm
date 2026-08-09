// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.memory.iterator;

import std;

import fmia.meta;

export namespace fmia {

// use when there is a high probability that p, q are equal
template <std::input_iterator I, std::strict_weak_order<std::iter_reference_t<I>, std::iter_reference_t<I>> Comparator = std::ranges::less>
[[nodiscard]] constexpr bool iter_compare(I p, I q, Comparator comp = std::ranges::less {}) noexcept(noexcept(comp(*p, *q))) {
  if (p == q)
    return false;
  return comp(*p, *q);
}

} // export namespace fmia
