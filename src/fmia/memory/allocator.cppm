// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.memory.allocator;

import std;

export namespace fmia {

// represent std::allocator<T>
struct std_allocator_tag {};

// represent std::pmr::polymorphic_allocator<T>
struct std_pmr_allocator_tag {};

} // export namespace fmia

namespace fmia {

enum class uninitialized_construction_category { move, copy };

template <typename Allocator, typename InputIt, typename ForwardIt>
concept uninitialized_construct_function_invocable =
  std::same_as<typename std::allocator_traits<Allocator>::value_type, std::iter_value_t<ForwardIt>>
  && std::constructible_from<typename std::allocator_traits<Allocator>::value_type, std::iter_reference_t<InputIt>>;

template <uninitialized_construction_category Category, typename Allocator, typename InputIt, typename ForwardIt>
constexpr auto uninitialized_construct_n(Allocator& alloc, InputIt first, std::size_t count, ForwardIt dest) {
  if (count == 0) {
    if constexpr (Category == uninitialized_construction_category::move)
      return std::pair {first, dest};
    else
      return dest;
  }

  using allocator_traits = std::allocator_traits<Allocator>;
  using value_type = allocator_traits::value_type;

  if !consteval {
    if constexpr (
      std::same_as<Allocator, std::allocator<value_type>> && std::is_trivially_copyable_v<value_type> && std::contiguous_iterator<InputIt>
      && std::contiguous_iterator<ForwardIt>
    ) {
      std::memcpy(std::to_address(dest), std::to_address(first), count * sizeof(value_type));
      if constexpr (Category == uninitialized_construction_category::move)
        return std::pair {first + static_cast<std::ptrdiff_t>(count), dest + static_cast<std::ptrdiff_t>(count)};
      else
        return dest + static_cast<std::ptrdiff_t>(count);
    }
  }

  auto cur = dest;
  try {
    for (; count > 0; ++first, (void)++cur) {
      --count;
      if constexpr (Category == uninitialized_construction_category::move)
        allocator_traits::construct(alloc, std::addressof(*cur), std::ranges::iter_move(first));
      else
        allocator_traits::construct(alloc, std::addressof(*cur), *first);
    }
  } catch (...) {
    for (; dest != cur; ++dest)
      allocator_traits::destroy(alloc, std::addressof(*dest));
    throw;
  }
  if constexpr (Category == uninitialized_construction_category::move)
    return std::pair {first, cur};
  else
    return cur;
}

} // namespace fmia

export namespace fmia {

template <typename Allocator, std::input_iterator InputIt, std::forward_iterator ForwardIt>
  requires uninitialized_construct_function_invocable<Allocator, InputIt, ForwardIt>
constexpr auto uninitialized_move_n(Allocator& alloc, InputIt first, std::size_t count, ForwardIt dest) {
  return uninitialized_construct_n<uninitialized_construction_category::move>(alloc, first, count, dest);
}

template <typename Allocator, std::input_iterator InputIt, std::forward_iterator ForwardIt>
  requires uninitialized_construct_function_invocable<Allocator, InputIt, ForwardIt>
constexpr auto uninitialized_copy_n(Allocator& alloc, InputIt first, std::size_t count, ForwardIt dest) {
  return uninitialized_construct_n<uninitialized_construction_category::copy>(alloc, first, count, dest);
}

} // export namespace fmia
