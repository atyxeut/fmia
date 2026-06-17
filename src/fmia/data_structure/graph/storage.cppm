// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.graph.storage;

import std;

import fmia.meta.arithmetic;

export namespace fmia::graph {

enum class direction { undirected, directed, mixed };

} // export namespace fmia::graph

namespace fmia::graph {

template <direction Direction, typename VertexSize, typename Weight>
struct common_trait_base
{
  static constexpr auto direction_tag = Direction;

  using vertex_size_type = VertexSize;
  using vertex_difference_type = std::make_signed_t<vertex_size_type>;

  using weight_type = Weight;

  [[nodiscard]] static consteval bool is_weighted() noexcept { return !std::same_as<Weight, void>; }
};

template <direction Direction, typename Id, typename VertexSize, typename Weight>
struct edge_trait_base : common_trait_base<Direction, VertexSize, Weight>
{
  using id_type = Id;

  [[nodiscard]] static consteval bool has_id() noexcept { return !std::same_as<id_type, void>; }
};

template <direction Direction, typename VertexSize, typename Weight, typename EdgeSize>
struct graph_trait_base : common_trait_base<Direction, VertexSize, Weight>
{
  using edge_size_type = EdgeSize;
  using edge_difference_type = std::make_signed_t<edge_size_type>;
};

} // namespace fmia::graph

export namespace fmia::graph {

template <typename>
struct edge_traits
{
};

template <typename T>
inline constexpr auto edge_direction_tag_v = edge_traits<std::remove_cvref_t<T>>::direction_tag;

template <typename T>
using edge_vertex_size_t = edge_traits<std::remove_cvref_t<T>>::vertex_size_type;

template <typename T>
using edge_vertex_difference_t = edge_traits<std::remove_cvref_t<T>>::vertex_difference_type;

template <typename T>
using edge_weight_t = edge_traits<std::remove_cvref_t<T>>::weight_type;

template <typename T>
using edge_id_t = edge_traits<std::remove_cvref_t<T>>::id_type;

template <typename>
struct graph_traits
{
};

template <typename T>
inline constexpr auto graph_direction_tag_v = graph_traits<std::remove_cvref_t<T>>::direction_tag;

template <typename T>
using graph_vertex_size_t = graph_traits<std::remove_cvref_t<T>>::vertex_size_type;

template <typename T>
using graph_edge_size_t = graph_traits<std::remove_cvref_t<T>>::edge_size_type;

template <typename T>
using graph_vertex_difference_t = graph_traits<std::remove_cvref_t<T>>::vertex_difference_type;

template <typename T>
using graph_edge_difference_t = graph_traits<std::remove_cvref_t<T>>::edge_difference_type;

template <typename T>
using graph_weight_t = graph_traits<std::remove_cvref_t<T>>::weight_type;

} // export namespace fmia::graph

export namespace fmia::graph {

template <typename T>
concept graph = requires(std::remove_cvref_t<T> g) {
  requires std::same_as<direction, decltype(graph_direction_tag_v<T>)>;

  typename graph_weight_t<T>;

  requires std::unsigned_integral<graph_vertex_size_t<T>>;
  { g.vertex_size() } -> std::same_as<graph_vertex_size_t<T>>;

  requires std::unsigned_integral<graph_edge_size_t<T>>;
  { g.edge_size() } -> std::same_as<graph_edge_size_t<T>>;

  requires std::signed_integral<graph_vertex_difference_t<T>>;
  requires std::same_as<std::make_unsigned_t<graph_vertex_difference_t<T>>, graph_vertex_size_t<T>>;

  requires std::signed_integral<graph_edge_difference_t<T>>;
  requires std::same_as<std::make_unsigned_t<graph_edge_difference_t<T>>, graph_edge_size_t<T>>;
};

template <typename T>
concept undirected_graph = graph<T> && graph_direction_tag_v<T> == direction::undirected;

template <typename T>
concept directed_graph = graph<T> && graph_direction_tag_v<T> == direction::directed;

template <typename T>
concept mixed_graph = graph<T> && graph_direction_tag_v<T> == direction::mixed;

template <typename T>
concept unweighted_graph = graph<T> && !graph_traits<std::remove_cvref_t<T>>::is_weighted();

template <typename T>
concept weighted_graph = graph<T> && graph_traits<std::remove_cvref_t<T>>::is_weighted();

} // export namespace fmia::graph

namespace fmia::graph {

template <typename>
struct edge_id;

template <>
struct edge_id<void>
{
};

template <std::unsigned_integral Id>
struct edge_id<Id>
{
  Id id;
};

template <typename>
struct edge_source;

template <>
struct edge_source<void>
{
};

template <std::unsigned_integral VertexSize>
struct edge_source<VertexSize>
{
  VertexSize u;
};

template <std::unsigned_integral VertexSize>
struct edge_destination
{
  VertexSize v;
};

template <typename>
struct edge_weight;

template <>
struct edge_weight<void>
{
  static constexpr int w = 1;
};

template <meta::arithmetic Weight>
struct edge_weight<Weight>
{
  Weight w;
};

template <direction Direction, typename Id, typename SourceVertexSize, std::unsigned_integral VertexSize, meta::arithmetic Weight>
  requires ((std::is_void_v<Id> || meta::precision_gteq<Id, VertexSize>)
            && (std::is_void_v<SourceVertexSize> || std::same_as<SourceVertexSize, VertexSize>))
class edge_base : public edge_id<Id>, public edge_source<SourceVertexSize>, public edge_destination<VertexSize>, public edge_weight<Weight>
{
};

}; // namespace fmia::graph

export namespace fmia::graph {

template <direction Direction, typename Id, typename SourceVertexSize, typename VertexSize, typename Weight>
struct edge_traits<edge_base<Direction, Id, SourceVertexSize, VertexSize, Weight>> : edge_trait_base<Direction, Id, VertexSize, Weight>
{
};

template <typename Id = void, typename SourceVertexSize = std::size_t, typename VertexSize = std::size_t, typename Weight = void>
using undirected_edge = edge_base<direction::undirected, Id, SourceVertexSize, VertexSize, Weight>;

template <typename Id = void, typename SourceVertexSize = std::size_t, typename VertexSize = std::size_t, typename Weight = void>
using directed_edge = edge_base<direction::directed, Id, SourceVertexSize, VertexSize, Weight>;

} // export namespace fmia::graph

export namespace fmia::graph {

template <typename Edge, typename EdgeSize = std::conditional_t<std::is_void_v<edge_id_t<Edge>>, edge_vertex_size_t<Edge>, edge_id_t<Edge>>>
  requires (
    (std::is_void_v<edge_id_t<Edge>> && meta::precision_gteq<EdgeSize, edge_vertex_size_t<Edge>>) || std::same_as<edge_id_t<Edge>, EdgeSize>
  )
class edge_list final : public graph_trait_base<edge_direction_tag_v<Edge>, edge_vertex_size_t<Edge>, edge_weight_t<Edge>, EdgeSize>
{
private:
  edge_vertex_size_t<Edge> vertex_count_;
  std::vector<Edge> edges_;

public:
  [[nodiscard]] constexpr bool empty() const noexcept { return edges_.empty(); }

  [[nodiscard]] constexpr auto vertex_size() const noexcept { return vertex_count_; }
  [[nodiscard]] constexpr auto edge_size() const noexcept { return edges_.size(); }

  [[nodiscard]] constexpr auto begin() noexcept { return edges_.begin(); }
  [[nodiscard]] constexpr auto begin() const noexcept { return edges_.begin(); }

  [[nodiscard]] constexpr auto end() noexcept { return edges_.end(); }
  [[nodiscard]] constexpr auto end() const noexcept { return edges_.end(); }

  [[nodiscard]] constexpr auto& operator [](EdgeSize idx) { return edges_[idx]; }
  [[nodiscard]] constexpr auto& operator [](EdgeSize idx) const { return edges_[idx]; }

  constexpr void reserve(EdgeSize capacity) { edges_.reserve(capacity); }

  template <typename... Args> constexpr void add_edge(Args&&... args) { edges_.emplace_back(std::forward<Args>(args)...); }
};

} // export namespace fmia::graph
