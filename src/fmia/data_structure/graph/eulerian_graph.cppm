// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.graph.eulerian_graph;

import std;

import fmia.data_structure.graph.storage;
import fmia.data_structure.graph.traverse;

export namespace fmia::graph {

// Eulerian trail: a trail that visits each edge exactly once
// Eulerian circuit: a closed Eulerian trail
//
// if a graph has an Eulerian trail, it is called a semi-Eulerian graph
// if a graph has an Eulerian circuit, it is called an Eulerian graph
//
// an undirected graph is semi-Eulerian iff:
// 1. ignoring isolated vertices, the graph is connected
// 2. exactly 0 or 2 vertices have odd degree
//
// a directed graph is semi-Eulerian iff:
// 1. ignoring isolated vertices, the graph is strongly connected
// 2. for any vertex u, deg+(u) = deg-(u), or exactly for two vertices u, v, deg+(u) + 1 = deg-(u) and deg+(v) = deg-(v) + 1
//
// an undirected graph is Eulerian iff:
// 1. the graph is semi-Eulerian
// 2. every vertex has even degree
//
// a directed graph is Eulerian iff:
// 1. the graph is semi-Eulerian
// 2. for each vertex u, deg+(u) = deg-(u)

enum class eulerian_graph_error { no_eulerian_trail, no_eulerian_circuit };

} // export namespace fmia::graph

namespace fmia::graph {

template <direction GraphDirection, typename Graph, typename Vertex = Graph::vertex_type>
[[nodiscard]] constexpr auto get_eulerian_trail_start_vertex(const Graph& g) noexcept -> std::pair<Vertex, bool> {
  const auto n = g.vertex_size();
  Vertex start = -1, end = -1;

  if constexpr (GraphDirection == direction::undirected) {
    for (Vertex u = 0; u < n; ++u) {
      if (g.degree(u) & 1) {
        if (start == -1)
          start = u;
        else if (end == -1)
          end = u;
        else
          return {-1, false};
      }
    }
  }

  if constexpr (GraphDirection == direction::directed) {
    for (Vertex u = 0; u < n; ++u) {
      const auto diff = g.in_degree(u) - g.out_degree(u);
      if (diff < -1 | diff > 1 | (diff == -1 & start != -1) | (diff == 1 & end != -1))
        return {-1, false};
      if (diff == -1)
        start = u;
      else if (diff == 1)
        end = u;
    }
  }

  // no Eulerian trails
  // for undirected graphs: missing the end
  // for directed graphs: missing the start or missing the end
  if (start == -1 ^ end == -1)
    return {-1, false};

  // has an Eulerian trail
  if (start != -1)
    return {start, false};

  // both the start and the end are -1, meaning:
  // for undirected graphs: every vertex has even degree
  // for directed graphs: for every vertex, its in degree = its out degree
  // check if there's an Eulerian circuit
  for (Vertex u = 0; u < n; ++u)
    if (g.degree(u) > 0)
      return {u, true};

  // every vertex is isolated, or the graph is a null graph, we consider it to have an empty Eulerian circuit
  return {-1, true};
}

template <typename Graph, typename Vertex = Graph::vertex_type, typename Iterator = Graph::neighbor_iterator>
[[nodiscard]] constexpr auto init_current_edge_iterators(const Graph& g) -> std::vector<Iterator> {
  const auto n = g.vertex_size();

  std::vector<Iterator> res(n);
  for (Vertex u = 0; u < n; ++u)
    res[u] = g.neighbors(u).begin();

  return res;
}

} // namespace fmia::graph

namespace fmia::graph {

// Hierholzer's algorithm
// time complexity: O(V + E)

template <typename G, typename T, typename Y, typename U, typename I>
constexpr void get_an_eulerian_trail_impl_for_undirected_recursive(const G& g, T u, Y& cur_edge_it, U& vis, I& path) {
  for (const auto end_it = g.neighbors(u).end(); cur_edge_it[u] != end_it;) {
    if (const auto [v, id] = *cur_edge_it[u]++; !vis[id]) {
      vis[id] = true;
      get_an_eulerian_trail_impl_for_undirected_recursive(g, v, cur_edge_it, vis, path);
    }
  }
  path.emplace_back(u);
}

template <typename G, typename T, typename Y, typename U, typename I>
constexpr void get_an_eulerian_trail_impl_for_undirected_iterative(const G& g, T start, Y& cur_edge_it, U& vis, I& path) {
  std::vector<T> stack {start};
  while (!stack.empty()) {
    const auto u = stack.back();
    if (const auto end_it = g.neighbors(u).end(); cur_edge_it[u] != end_it) {
      if (const auto [v, id] = *cur_edge_it[u]++; !vis[id]) {
        vis[id] = true;
        stack.emplace_back(v);
      }
    } else {
      path.emplace_back(u);
      stack.pop_back();
    }
  }
}

template <typename G, typename T, typename Y, typename U>
constexpr void get_an_eulerian_trail_impl_for_directed_recursive(const G& g, T u, Y& cur_edge_it, U& path) {
  for (const auto end_it = g.neighbors(u).end(); cur_edge_it[u] != end_it;)
    get_an_eulerian_trail_impl_for_directed_recursive(g, *cur_edge_it[u]++, cur_edge_it, path);
  path.emplace_back(u);
}

template <typename G, typename T, typename Y, typename U>
constexpr void get_an_eulerian_trail_impl_for_directed_iterative(const G& g, T start, Y& cur_edge_it, U& path) {
  std::vector<T> stack {start};
  while (!stack.empty()) {
    const auto u = stack.back();
    if (const auto end_it = g.neighbors(u).end(); cur_edge_it[u] != end_it)
      stack.emplace_back(*cur_edge_it[u]++);
    else {
      path.emplace_back(u);
      stack.pop_back();
    }
  }
}

template <direction GraphDirection, typename T, typename G, typename U>
constexpr void get_an_eulerian_trail_impl(T& path, const G& g, U start) {
  auto cur_edge_it = init_current_edge_iterators(g);

  if constexpr (GraphDirection == direction::undirected) {
    std::vector<bool> vis(g.edge_size() >> 1);
    get_an_eulerian_trail_impl_for_undirected_iterative(g, start, cur_edge_it, vis, path);
  }

  if constexpr (GraphDirection == direction::directed)
    get_an_eulerian_trail_impl_for_directed_iterative(g, start, cur_edge_it, path);
}

template <graph_trail_tag TrailTag, direction GraphDirection, typename Graph, typename Vertex = Graph::vertex_type>
[[nodiscard]] constexpr auto get_an_eulerian_trail(const Graph& g) -> std::expected<std::vector<Vertex>, eulerian_graph_error> {
  std::vector<Vertex> path;

  const auto [start, is_circuit] = get_eulerian_trail_start_vertex<GraphDirection>(g);

  if constexpr (TrailTag == graph_trail_tag::circuit)
    if (!is_circuit)
      return std::unexpected(eulerian_graph_error::no_eulerian_circuit);

  if (start == -1) {
    if (is_circuit)
      return path;
    else
      return std::unexpected(eulerian_graph_error::no_eulerian_trail);
  }

  get_an_eulerian_trail_impl<GraphDirection>(path, g, start);
  std::ranges::reverse(path);
  return path;
}

} // namespace fmia::graph

export namespace fmia::graph {

// leave isolated vertices, the rest of the graph must be (strongly) connected, otherwise the following functions have undefined behavior

template <graph T>
[[nodiscard]] constexpr auto get_an_eulerian_trail_for_undirected(const T& g) {
  return get_an_eulerian_trail<graph_trail_tag::none, direction::undirected>(g);
}

template <graph T>
[[nodiscard]] constexpr auto get_an_eulerian_circuit_for_undirected(const T& g) {
  return get_an_eulerian_trail<graph_trail_tag::circuit, direction::undirected>(g);
}

template <graph T>
[[nodiscard]] constexpr auto get_an_eulerian_trail_for_directed(const T& g) {
  return get_an_eulerian_trail<graph_trail_tag::none, direction::directed>(g);
}

template <graph T>
[[nodiscard]] constexpr auto get_an_eulerian_circuit_for_directed(const T& g) {
  return get_an_eulerian_trail<graph_trail_tag::circuit, direction::directed>(g);
}

} // export namespace fmia::graph
