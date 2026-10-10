// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.misc.n_queens;

import fmia.math.integer;

export namespace fmia {

u64 n_queens(u32 n) {
  if (n == 0)
    return 1;

  const auto f = [finish = (1u << n) - 1](this const auto f, u32 row, u32 diag, u32 adiag) -> u64 {
    if (row == finish)
      return 1;
    u64 cnt = 0;
    for (u32 avail = finish & ~(row | diag | adiag); avail; avail &= avail - 1) {
      const u32 cur = avail & -avail;
      cnt += f(row | cur, (diag | cur) << 1, (adiag | cur) >> 1);
    }
    return cnt;
  };

  u64 ans = 0;
  const u32 last = 1 << n / 2;
  if (n & 1)
    ans += f(last, last << 1, last >> 1);
  for (u32 cur = 1; cur != last; cur <<= 1)
    ans += 2 * f(cur, cur << 1, cur >> 1);
  return ans;
}

} // namespace fmia
