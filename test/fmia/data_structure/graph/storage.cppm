// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.data_structure.graph.storage;

import std;

import fmia.data_structure.graph.storage;

using namespace fmia;

export {

consteval void edge_class_aggregate_initialization() noexcept {
  using T0 = graph::undirected_edge<std::size_t, std::size_t, std::size_t, int>;
  static_assert(std::is_aggregate_v<T0>);
  T0 complete_edge {0, 5, 6, 998244353};

  // invalid: cannot specify a base class member
  // T0 complete_edge {.id = 0, .u = 5, .v = 6, .w = 998244353};

  using T1 = graph::undirected_edge<void, std::size_t, std::size_t, double>;
  static_assert(std::is_aggregate_v<T1>);
  T1 no_id_edge {{}, 1, 3, 3.14};

  using T2 = graph::undirected_edge<void, void, std::size_t, int>;
  static_assert(std::is_aggregate_v<T2>);
  T2 no_source_edge {{}, {}, 3, 1000};

  using T3 = graph::undirected_edge<std::size_t, void, std::size_t, void>;
  static_assert(std::is_aggregate_v<T3>);

  // T3 has_id_no_weight_edge {0, {}, 3, {}};

  // can omit the last empty initializer
  T3 has_id_no_weight_edge {0, {}, 3};
}

} // export
