// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.random.distribution;

import std;

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

} // export namespace fmia::random
