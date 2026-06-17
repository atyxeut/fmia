// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.graph.storage;

import std;

import fmia.meta.arithmetic;

export namespace fmia::graph {

enum class graph_direction { undirected, directed, mixed };
enum class graph_weight { unweighted, weighted };

} // export namespace fmia::graph

namespace fmia::graph {

template <typename T>
concept internal_graph = requires {
  typename T::vertex_size_type;
  typename T::edge_size_type;
  T::direction_tag;
  T::weight_tag;
};

} // namespace fmia::graph

export namespace fmia::graph {

template <typename>
struct graph_traits
{
};

template <internal_graph T>
struct graph_traits<T>
{
  static constexpr auto direction_tag = T::direction_tag;
  static constexpr auto weight_tag = T::weight_tag;

  using vertex_size_type = T::vertex_size_type;
  using edge_size_type = T::edge_size_type;

  using vertex_difference_type = std::make_signed_t<vertex_size_type>;
  using edge_difference_type = std::make_signed_t<edge_size_type>;
};

template <typename T>
inline constexpr auto graph_direction_tag_v = graph_traits<std::remove_cvref_t<T>>::direction_tag;

template <typename T>
inline constexpr auto graph_weight_tag_v = graph_traits<std::remove_cvref_t<T>>::weight_tag;

template <typename T>
using graph_vertex_size_t = graph_traits<std::remove_cvref_t<T>>::vertex_size_type;

template <typename T>
using graph_vertex_difference_t = graph_traits<std::remove_cvref_t<T>>::vertex_difference_type;

template <typename T>
using graph_edge_size_t = graph_traits<std::remove_cvref_t<T>>::edge_size_type;

template <typename T>
using graph_edge_difference_t = graph_traits<std::remove_cvref_t<T>>::edge_difference_type;

template <typename T>
concept graph = requires(std::remove_cvref_t<T> g) {
  requires std::same_as<graph_direction, decltype(graph_direction_tag_v<T>)>;
  requires std::same_as<graph_weight, decltype(graph_weight_tag_v<T>)>;

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
concept undirected_graph = graph<T> && graph_direction_tag_v<T> == graph_direction::undirected;

template <typename T>
concept directed_graph = graph<T> && graph_direction_tag_v<T> == graph_direction::directed;

template <typename T>
concept mixed_graph = graph<T> && graph_direction_tag_v<T> == graph_direction::mixed;

template <typename T>
concept unweighted_graph = graph<T> && graph_weight_tag_v<T> == graph_weight::unweighted;

template <typename T>
concept weighted_graph = graph<T> && graph_weight_tag_v<T> == graph_weight::weighted;

} // export namespace fmia::graph

namespace fmia::graph {

template <typename Vertex>
struct edge_from_base
{
  Vertex u;
};

template <typename Vertex>
struct edge_to_base
{
  Vertex v;
};

template <typename Vertex>
struct edge_base : edge_from_base<Vertex>, edge_to_base<Vertex>
{
};

template <typename Degree>
struct edge_id
{
  Degree id;
};

} // namespace fmia::graph

export namespace fmia::graph {

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree>
struct unweighted_edge_from : edge_from_base<Vertex>, edge_id<Degree>
{
};

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree>
struct unweighted_edge_to : edge_to_base<Vertex>, edge_id<Degree>
{
};

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree>
struct unweighted_edge : edge_base<Vertex>, edge_id<Degree>
{
};

} // export namespace fmia::graph

namespace fmia::graph {

template <typename>
struct weight;

template <>
struct weight<void>
{
  static constexpr int w = 1;
};

template <meta::arithmetic T>
struct weight<T>
{
  static constexpr auto infinity = std::numeric_limits<T>::max();
  static constexpr auto negative_infinity = std::numeric_limits<T>::min();

  T w;
};

} // namespace fmia::graph

export namespace fmia::graph {

template <std::signed_integral Vertex, typename Weight>
struct weighted_edge_from : edge_from_base<Vertex>, weight<Weight>
{
};

template <std::signed_integral Vertex, typename Weight>
struct weighted_edge_to : edge_to_base<Vertex>, weight<Weight>
{
};

template <std::signed_integral Vertex, typename Weight>
struct weighted_edge : edge_base<Vertex>, weight<Weight>
{
};

}; // export namespace fmia::graph

export namespace fmia::meta {

template <typename T>
struct is_no_cv_edge_list : std::false_type
{
};

template <typename T>
inline constexpr bool is_no_cv_edge_list_v = is_no_cv_edge_list<T>::value;

template <typename T>
concept edge_list = is_no_cv_edge_list_v<std::remove_cvref_t<T>>;

template <typename T>
struct is_no_cv_basic_edge_list : std::false_type
{
};

template <typename T>
inline constexpr bool is_no_cv_basic_edge_list_v = is_no_cv_basic_edge_list<T>::value || is_no_cv_edge_list_v<T>;

template <typename T>
concept basic_edge_list = is_no_cv_basic_edge_list_v<std::remove_cvref_t<T>>;

} // export namespace fmia::meta

namespace fmia::graph {

template <typename Vertex, typename Edge, typename Degree>
class basic_edge_list_base
{
public:
  using vertex_type = Vertex;
  using edge_type = Edge;
  using degree_type = Degree;

protected:
  using storage_type_ = std::vector<Edge>;
  using storage_size_type_ = storage_type_::size_type;

  storage_type_ edges_;

public:
  [[nodiscard]] constexpr bool empty() const noexcept { return edges_.empty(); }

  [[nodiscard]] constexpr auto edge_size() const noexcept { return edges_.size(); }

  [[nodiscard]] constexpr auto begin() noexcept { return edges_.begin(); }
  [[nodiscard]] constexpr auto begin() const noexcept { return edges_.begin(); }

  [[nodiscard]] constexpr auto end() noexcept { return edges_.end(); }
  [[nodiscard]] constexpr auto end() const noexcept { return edges_.end(); }

  [[nodiscard]] constexpr auto& operator [](storage_size_type_ idx) { return edges_[idx]; }
  [[nodiscard]] constexpr auto& operator [](storage_size_type_ idx) const { return edges_[idx]; }

  constexpr void reserve(storage_size_type_ capacity) { edges_.reserve(capacity); }
};

template <typename Vertex, typename Edge, typename Degree>
class edge_list_base : public basic_edge_list_base<Vertex, Edge, Degree>
{
private:
  using base_ = basic_edge_list_base<Vertex, Edge, Degree>;

protected:
  std::unordered_map<Vertex, bool> exist_;

public:
  [[nodiscard]] constexpr auto vertex_size() const noexcept { return static_cast<Vertex>(exist_.size()); }
};

} // namespace fmia::graph

export namespace fmia::graph {

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree = meta::make_higher_precision_t<Vertex>>
struct basic_unweighted_edge_list : public basic_edge_list_base<Vertex, unweighted_edge<Vertex, Degree>, Degree>
{
  constexpr void add_edge(Vertex u, Vertex v) { this->edges_.emplace_back(u, v, this->edge_size()); }
};

template <std::signed_integral Vertex, meta::arithmetic Weight, meta::precision_gteq<Vertex> Degree = meta::make_higher_precision_t<Vertex>>
struct basic_weighted_edge_list : public basic_edge_list_base<Vertex, weighted_edge<Vertex, Weight>, Degree>
{
  using weight_type = Weight;

  constexpr void add_edge(Vertex u, Vertex v, const Weight& w) { this->edges_.emplace_back(u, v, w); }
  constexpr void add_edge(Vertex u, Vertex v, Weight&& w) { this->edges_.emplace_back(u, v, std::move(w)); }
};

} // export namespace fmia::graph

export namespace fmia::meta {

template <typename Vertex, typename Degree>
struct is_no_cv_basic_edge_list<graph::basic_unweighted_edge_list<Vertex, Degree>> : std::true_type
{
};

template <typename Vertex, typename Weight, typename Degree>
struct is_no_cv_basic_edge_list<graph::basic_weighted_edge_list<Vertex, Weight, Degree>> : std::true_type
{
};

} // export namespace fmia::meta

export namespace fmia::graph {

template <std::signed_integral Vertex, meta::precision_gteq<Vertex> Degree = meta::make_higher_precision_t<Vertex>>
class unweighted_edge_list : public edge_list_base<Vertex, unweighted_edge<Vertex, Degree>, Degree>
{
public:
  constexpr void add_edge(Vertex u, Vertex v)
  {
    this->exist_[u] = this->exist_[v] = true;
    this->edges_.emplace_back(u, v, this->edge_size());
  }
};

template <
  std::signed_integral Vertex, meta::arithmetic Weight, meta::precision_gteq<Vertex> Degree = meta::make_higher_precision_t<Vertex>
> class weighted_edge_list : public edge_list_base<Vertex, weighted_edge<Vertex, Weight>, Degree>
{
public:
  using weight_type = Weight;

  constexpr void add_edge(Vertex u, Vertex v, const Weight& w)
  {
    this->exist_[u] = this->exist_[v] = true;
    this->edges_.emplace_back(u, v, w);
  }

  constexpr void add_edge(Vertex u, Vertex v, Weight&& w)
  {
    this->exist_[u] = this->exist_[v] = true;
    this->edges_.emplace_back(u, v, std::move(w));
  }
};

} // export namespace fmia::graph
