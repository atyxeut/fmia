// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.graph.storage;

import std;

import fmia.meta.arithmetic;

export namespace fmia::graph {

enum class direction { undirected, directed, mixed };

} // export namespace fmia::graph

namespace fmia::graph {

template <direction Direction, typename Order, typename Weight>
struct common_trait_base {
  static constexpr auto direction_tag = Direction;

  using order_type = Order;
  using order_difference_type = std::make_signed_t<order_type>;

  using weight_type = Weight;
  [[nodiscard]] static consteval bool is_weighted() noexcept { return !std::same_as<Weight, void>; }
};

template <direction Direction, typename Id, typename Order, typename Weight>
struct edge_trait_base : common_trait_base<Direction, Order, Weight> {
  using id_type = Id;
  [[nodiscard]] static consteval bool has_id() noexcept { return !std::same_as<id_type, void>; }
};

template <direction Direction, typename Order, typename Weight, typename Size>
struct graph_trait_base : common_trait_base<Direction, Order, Weight> {
  using size_type = Size;
  using size_difference_type = std::make_signed_t<size_type>;
};

} // namespace fmia::graph

export namespace fmia::graph {

template <typename>
struct edge_traits {};

template <typename T>
inline constexpr auto edge_direction_v = edge_traits<std::remove_cvref_t<T>>::direction_tag;

template <typename T>
using edge_order_t = edge_traits<std::remove_cvref_t<T>>::order_type;

template <typename T>
using edge_order_difference_t = edge_traits<std::remove_cvref_t<T>>::order_difference_type;

template <typename T>
using edge_weight_t = edge_traits<std::remove_cvref_t<T>>::weight_type;

template <typename T>
using edge_id_t = edge_traits<std::remove_cvref_t<T>>::id_type;

template <typename>
struct graph_traits {};

template <typename T>
inline constexpr auto graph_direction_v = graph_traits<std::remove_cvref_t<T>>::direction_tag;

template <typename T>
using graph_order_t = graph_traits<std::remove_cvref_t<T>>::order_type;

template <typename T>
using graph_size_t = graph_traits<std::remove_cvref_t<T>>::size_type;

template <typename T>
using graph_order_difference_t = graph_traits<std::remove_cvref_t<T>>::order_difference_type;

template <typename T>
using graph_size_difference_t = graph_traits<std::remove_cvref_t<T>>::size_difference_type;

template <typename T>
using graph_weight_t = graph_traits<std::remove_cvref_t<T>>::weight_type;

} // export namespace fmia::graph

export namespace fmia::graph {

template <typename T>
concept graph = requires(std::remove_cvref_t<T> g) {
  requires std::same_as<direction, decltype(graph_direction_v<T>)>;

  requires std::unsigned_integral<graph_order_t<T>>;
  { g.order() } -> std::same_as<graph_order_t<T>>;
  requires std::signed_integral<graph_order_difference_t<T>>;
  requires std::same_as<std::make_unsigned_t<graph_order_difference_t<T>>, graph_order_t<T>>;

  requires std::unsigned_integral<graph_size_t<T>>;
  { g.size() } -> std::same_as<graph_size_t<T>>;
  requires std::signed_integral<graph_size_difference_t<T>>;
  requires std::same_as<std::make_unsigned_t<graph_size_difference_t<T>>, graph_size_t<T>>;

  requires meta::arithmetic<graph_weight_t<T>>;
};

template <typename T>
concept undirected_graph = graph<T> && graph_direction_v<T> == direction::undirected;

template <typename T>
concept directed_graph = graph<T> && graph_direction_v<T> == direction::directed;

template <typename T>
concept mixed_graph = graph<T> && graph_direction_v<T> == direction::mixed;

template <typename T>
concept unweighted_graph = graph<T> && !graph_traits<std::remove_cvref_t<T>>::is_weighted();

template <typename T>
concept weighted_graph = graph<T> && graph_traits<std::remove_cvref_t<T>>::is_weighted();

} // export namespace fmia::graph

namespace fmia::graph {

template <typename>
struct edge_id;

template <>
struct edge_id<void> {};

template <std::unsigned_integral Id>
struct edge_id<Id> {
  Id id;
};

template <typename>
struct edge_source;

template <>
struct edge_source<void> {};

template <std::unsigned_integral Order>
struct edge_source<Order> {
  Order u;
};

template <std::unsigned_integral Order>
struct edge_destination {
  Order v;
};

template <typename>
struct edge_weight;

template <>
struct edge_weight<void> {
  static constexpr int w = 1;
};

template <meta::arithmetic Weight>
struct edge_weight<Weight> {
  Weight w;
};

template <direction Direction, typename Id, typename SourceOrder, std::unsigned_integral Order, typename Weight>
  requires ((std::is_void_v<Id> || meta::precision_gteq<Id, Order>)
    && (std::is_void_v<SourceOrder> || std::same_as<SourceOrder, Order>))
    && (std::is_void_v<Weight> || meta::arithmetic<Weight>)
class edge_base
  : public edge_id<Id>,
    public edge_source<SourceOrder>,
    public edge_destination<Order>,
    public edge_weight<Weight> {};

}; // namespace fmia::graph

export namespace fmia::graph {

template <direction Direction, typename Id, typename SourceOrder, typename Order, typename Weight>
struct edge_traits<edge_base<Direction, Id, SourceOrder, Order, Weight>> : edge_trait_base<Direction, Id, Order, Weight> {};

template <typename Id = void, typename SourceOrder = std::size_t, typename Order = std::size_t, typename Weight = void>
using undirected_edge = edge_base<direction::undirected, Id, SourceOrder, Order, Weight>;

template <typename Id = void, typename SourceOrder = std::size_t, typename Order = std::size_t, typename Weight = void>
using directed_edge = edge_base<direction::directed, Id, SourceOrder, Order, Weight>;

} // export namespace fmia::graph

export namespace fmia::graph {

template <typename Edge, typename Size = std::conditional_t<std::is_void_v<edge_id_t<Edge>>, edge_order_t<Edge>, edge_id_t<Edge>>>
  requires (
    (std::is_void_v<edge_id_t<Edge>> && meta::precision_gteq<Size, edge_order_t<Edge>>) || std::same_as<edge_id_t<Edge>, Size>
  )
class edge_list final : public graph_trait_base<edge_direction_v<Edge>, edge_order_t<Edge>, edge_weight_t<Edge>, Size> {
private:
  edge_order_t<Edge> order_;
  std::vector<Edge> edges_;

public:
  [[nodiscard]] constexpr bool empty() const noexcept { return edges_.empty(); }

  [[nodiscard]] constexpr auto order() const noexcept { return order_; }
  [[nodiscard]] constexpr auto size() const noexcept { return edges_.size(); }

  [[nodiscard]] constexpr auto begin() noexcept { return edges_.begin(); }
  [[nodiscard]] constexpr auto begin() const noexcept { return edges_.begin(); }

  [[nodiscard]] constexpr auto end() noexcept { return edges_.end(); }
  [[nodiscard]] constexpr auto end() const noexcept { return edges_.end(); }

  [[nodiscard]] constexpr auto& operator [](Size idx) { return edges_[idx]; }
  [[nodiscard]] constexpr auto& operator [](Size idx) const { return edges_[idx]; }

  constexpr void reserve(Size capacity) { edges_.reserve(capacity); }

  template <typename... Args> constexpr void add_edge(Args&&... args) { edges_.emplace_back(std::forward<Args>(args)...); }
};

} // export namespace fmia::graph
