// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.binary_tree.scapegoat_tree;

import std;

export namespace fmia {

template <std::size_t N, typename Key, double Alpha = 0.70, typename Comparator = std::ranges::less>
class inplace_scapegoat_tree {};

} // export namespace fmia
