// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.util;

import std;

import fmia.util;

using namespace fmia;

export {

void update_max_functionality() {
  int a = 10, b = 20;

  const bool res_0 = update_max(a, b);
  contract_assert(res_0 == true);

  const bool res_1 = update_max(a, b);
  contract_assert(res_1 == false);

  const bool res_2 = update_max(a, 30);
  contract_assert(res_2 == true);

  double c = 1.23, d = 12.3;
  const bool res_3 = update_max(c, d);
  contract_assert(res_3 == true);

  const bool res_4 = update_max(c, d);
  contract_assert(res_4 == false);

  const bool res_5 = update_max(c, 15.0);
  contract_assert(res_5 == true);
}

void update_min_functionality() {
  int a = 10, b = 5;

  const bool res_0 = update_min(a, b);
  contract_assert(res_0 == true);

  const bool res_1 = update_min(a, b);
  contract_assert(res_1 == false);

  const bool res_2 = update_min(a, 1);
  contract_assert(res_2 == true);

  double c = 12.3, d = 3.14;
  const bool res_3 = update_min(c, d);
  contract_assert(res_3 == true);

  const bool res_4 = update_min(c, d);
  contract_assert(res_4 == false);

  const bool res_5 = update_min(c, 1.23);
  contract_assert(res_5 == true);
}

} // export
