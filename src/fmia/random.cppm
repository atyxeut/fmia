// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.random;

import std;

import fmia.data_structure.graph.storage;
import fmia.data_structure.string.character.ascii;

export namespace fmia::random {

inline const auto time_seed = std::chrono::steady_clock::now().time_since_epoch().count();
inline std::random_device seed_generator;

inline std::mt19937 mt19937_engine(seed_generator());
inline std::mt19937_64 mt19937_64_engine(seed_generator());

template <std::integral T>
[[nodiscard]] auto uniform_distribution(T l, T r) {
  return std::uniform_int_distribution<T>(l, r);
}

template <std::floating_point T>
[[nodiscard]] auto uniform_distribution(T l, T r) {
  return std::uniform_real_distribution<T>(l, r);
}

template <typename T, typename Engine = std::mt19937>
  requires std::is_arithmetic_v<T>
[[nodiscard]] auto rand(T l, T r, Engine& engine = mt19937_engine) {
  return uniform_distribution(l, r)(engine);
}

} // export namespace fmia::random

namespace fmia::random {

template <bool AllowNegative>
[[nodiscard]] std::string rand_integer_impl(std::size_t length) {
  if (length == 0)
    throw std::invalid_argument("length must be positive");

  std::string data;
  data.reserve(length + AllowNegative);

  if constexpr (AllowNegative)
    if (rand(0uz, 1uz))
      data += '-';

  data += radix_digits<10>[rand(1uz, 9uz)];
  for (auto dist = uniform_distribution(0uz, 9uz); length-- > 1;)
    data += radix_digits<10>[dist(mt19937_engine)];

  return data;
}

} // namespace fmia::random

export namespace fmia::random {

[[nodiscard]] auto rand_positive_integer(std::size_t length) {
  return rand_integer_impl<false>(length);
}

[[nodiscard]] auto rand_integer(std::size_t length) {
  return rand_integer_impl<true>(length);
}

template <std::integral T>
[[nodiscard]] std::vector<T> rand_permutation(T begin, T end) {
  if (begin > end)
    throw std::invalid_argument("invalid integer range");

  std::vector<T> data(end - begin + 1);
  std::iota(data.begin(), data.end(), begin);
  std::ranges::shuffle(data, mt19937_engine);
  return data;
}

template <typename T, typename Fn>
[[nodiscard]] std::vector<T> rand_vector(std::size_t count, Fn&& fn) {
  std::vector<T> res(count);
  std::ranges::generate(res, std::forward<Fn>(fn));
  std::ranges::shuffle(res, mt19937_engine);
  return res;
}

template <typename T = int>
  requires std::is_arithmetic_v<T>
[[nodiscard]] auto rand_number_vector(std::size_t count, T begin = std::numeric_limits<T>::min(), T end = std::numeric_limits<T>::max()) {
  return rand_vector<T>(count, [=] { return rand(begin, end); });
}

// get the edge list of a random unweighted tree
// template <bool GenerateStar = false, std::integral Vertex>
// [[nodiscard]] auto rand_unweighted_tree(Vertex vertex_begin, Vertex vertex_end) -> graph::basic_unweighted_edge_list<Vertex> {
//   if (vertex_begin > vertex_end)
//     throw std::invalid_argument("invalid vertex index range");

//   const auto p = permutation(vertex_begin, vertex_end);

//   const auto edge_cnt = vertex_end - vertex_begin;
//   graph::basic_unweighted_edge_list<Vertex> data;
//   data.reserve(edge_cnt);

//   for (auto v = 1uz; v <= edge_cnt; ++v) {
//     if constexpr (GenerateStar)
//       data.add_edge(p[0], p[v]);
//     else
//       data.add_edge(p[rand(0uz, v - 1)], p[v]);
//   }

//   return data;
// }

// get the edge list of a random weighted tree
// template <bool GenerateStar = false, std::integral Vertex, std::integral Weight>
// [[nodiscard]] auto rand_weighted_tree(Vertex vertex_begin, Vertex vertex_end, Weight weight_begin, Weight weight_end)
//   -> graph::basic_weighted_edge_list<Vertex, Weight> {
//   if (vertex_begin > vertex_end)
//     throw std::invalid_argument("invalid vertex index range");

//   if (weight_begin > weight_end)
//     throw std::invalid_argument("invalid weight range");

//   const auto p = permutation(vertex_begin, vertex_end);

//   const auto edge_cnt = vertex_end - vertex_begin;
//   graph::basic_weighted_edge_list<Vertex, Weight> data;
//   data.reserve(edge_cnt);

//   auto w_dist = uniform_dist(weight_begin, weight_end);
//   for (auto v = 1uz; v <= edge_cnt; ++v) {
//     if constexpr (GenerateStar)
//       data.add_edge(p[0], p[v], w_dist(mt19937_engine));
//     else
//       data.add_edge(p[rand(0uz, v - 1)], p[v], w_dist(mt19937_engine));
//   }

//   return data;
// }

} // export namespace fmia::random
