// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.graph.traverse;

import std;

import fmia.data_structure.graph.storage;

export namespace fmia::graph {

// walk: a sequence of vertices and edges, must begin with a vertex and also end with a vertex
// closed walk: a walk whose start is the same as the end
// open walk: a walk whose start and end are different
//
// trail: a walk that has no duplicate edges
// circuit: a closed trail
//
// path: a walk that has no duplicate vertices (which implies no duplicate edges)
// cycle: a closed path

enum class graph_trail_tag { none, circuit };
enum class graph_path_tag { none, cycle };

} // export namespace fmia::graph

export namespace fmia::graph {

enum class toposort_error { has_cycle };

} // export namespace fmia::graph

namespace fmia::graph {

enum class toposort_tag { none, lexicographical };

template <toposort_tag Order, typename Graph, typename Fn>
[[nodiscard]] constexpr auto toposort_impl(const Graph& g, Fn&& fn) -> std::expected<bool, toposort_error>
{
  using vertex_type = Graph::vertex_type;

  using queue_type = std::conditional_t<
    Order == toposort_tag::lexicographical, std::priority_queue<vertex_type, std::vector<vertex_type>, std::greater<vertex_type>>,
    std::queue<vertex_type>
  >;

  vertex_type n = g.vertex_size();

  queue_type q;
  for (vertex_type u = 0; u < n; ++u)
    if (g.in_degree(u) == 0)
      q.push(u);

  bool order_unique = true;

  while (!q.empty()) {
    if (q.size() > 1)
      order_unique = false;

    const auto u = [&] {
      if constexpr (std::same_as<queue_type, std::queue<vertex_type>>)
        return q.front();
      else
        return q.top();
    }();
    q.pop();
    --n;

    std::invoke(fn, u);

    for (const auto v : g.neighbors(u))
      if (--g.in_degree(v) == 0)
        q.push(v);
  }

  if (n != 0)
    return std::unexpected(toposort_error::has_cycle);

  return order_unique;
}

} // namespace fmia::graph

export namespace fmia::graph {

template <graph T, typename Fn>
[[nodiscard]] constexpr auto toposort(const T& g, Fn&& fn)
{
  return toposort_impl<toposort_tag::none>(g, std::forward<Fn>(fn));
}

template <graph T, typename Fn>
[[nodiscard]] constexpr auto toposort_lexicographical(const T& g, Fn&& fn)
{
  return toposort_impl<toposort_tag::lexicographical>(g, std::forward<Fn>(fn));
}

} // export namespace fmia::graph
