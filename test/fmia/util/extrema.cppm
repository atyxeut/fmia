// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.util.extrema;

import std;

import fmia.util.extrema;

using namespace fmia;

export {

void update_max_functionality() {
  int a = 10, b = 20;

  const bool res0 = update_max(a, b);
  contract_assert(res0 == true);

  const bool res1 = update_max(a, b);
  contract_assert(res1 == false);

  const bool res2 = update_max(a, 30);
  contract_assert(res2 == true);

  double c = 1.23, d = 12.3;
  const bool res3 = update_max(c, d);
  contract_assert(res3 == true);

  const bool res4 = update_max(c, d);
  contract_assert(res4 == false);

  const bool res5 = update_max(c, 15.0);
  contract_assert(res5 == true);
}

void update_min_functionality() {
  int a = 10, b = 5;

  const bool res0 = update_min(a, b);
  contract_assert(res0 == true);

  const bool res1 = update_min(a, b);
  contract_assert(res1 == false);

  const bool res2 = update_min(a, 1);
  contract_assert(res2 == true);

  double c = 12.3, d = 3.14;
  const bool res3 = update_min(c, d);
  contract_assert(res3 == true);

  const bool res4 = update_min(c, d);
  contract_assert(res4 == false);

  const bool res5 = update_min(c, 1.23);
  contract_assert(res5 == true);
}

} // export
