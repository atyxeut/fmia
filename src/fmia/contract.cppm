// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.contract;

import std;

export void handle_contract_violation(const std::contracts::contract_violation&) {
  std::println(std::cerr, "contract violation detected");
}
