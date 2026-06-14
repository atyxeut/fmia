// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.graph.connectivity;

import std;

import fmia.data_structure.graph.storage;

export namespace fmia::graph {

template <graph T, typename V = T::vertex_type>
[[nodiscard]] constexpr std::vector<V> get_scc_tarjan_recursive(const T& g) {
  std::vector<V> dfn(g.vertex_size()), low(g.vertex_size()), scc_id(g.vertex_size());
  V cur_dfn = 0, scc_size = 0;

  std::vector<V> stk;

  const auto dfs = [&](this const auto& self, V u) -> void {
    low[u] = dfn[u] = ++cur_dfn;
    stk.push_back(u);

    for (const auto v : g.neighbors(u)) {
      if (scc_id[v] != 0)
        continue;
      if (dfn[v] != 0) {
        self(v);
        low[u] = std::min(low[u], low[v]);
      } else {
        low[u] = std::min(low[u], dfn[v]);
      }
    }

    if (low[u] == dfn[u]) {
      ++scc_size;
      V v;
      do {
        v = stk.back();
        stk.pop_back();
        scc_id[v] = scc_size;
      } while (v != u);
    }
  };

  for (const auto u : g.vertices())
    if (dfn[u] == 0)
      dfs(u);

  return scc_id;
}

} // export namespace fmia::graph
