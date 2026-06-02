// Copyright (C) 2026 atyxeut
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <https://www.gnu.org/licenses/>.

export module fmia.memory.storage_base;

import std;

import fmia.math.integer.fixed_precision;

export namespace fmia {

enum class storage_location { heap, inplace };
enum class exception_safety { basic, strong };

} // export namespace fmia

namespace fmia {

enum class uninitialized_construction_category { move, copy };

template <typename Allocator, typename InputIt, typename ForwardIt>
concept uninitialized_construct_function_invocable =
  std::same_as<typename std::allocator_traits<Allocator>::value_type, std::iter_value_t<ForwardIt>>
  && std::constructible_from<typename std::allocator_traits<Allocator>::value_type, std::iter_reference_t<InputIt>>;

template <uninitialized_construction_category Category, typename Allocator, typename InputIt, typename ForwardIt>
constexpr auto uninitialized_construct_n(Allocator& alloc, InputIt first, std::size_t count, ForwardIt dest)
{
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
constexpr auto uninitialized_move_n(Allocator& alloc, InputIt first, std::size_t count, ForwardIt dest)
{
  return uninitialized_construct_n<uninitialized_construction_category::move>(alloc, first, count, dest);
}

template <typename Allocator, std::input_iterator InputIt, std::forward_iterator ForwardIt>
  requires uninitialized_construct_function_invocable<Allocator, InputIt, ForwardIt>
constexpr auto uninitialized_copy_n(Allocator& alloc, InputIt first, std::size_t count, ForwardIt dest)
{
  return uninitialized_construct_n<uninitialized_construction_category::copy>(alloc, first, count, dest);
}

} // export namespace fmia

namespace fmia {

template <meta::size_integral T>
constexpr T dynamic_storage_capacity = static_cast<T>(-1);

template <typename Size, Size Capacity>
struct heap_capacity
{
  static constexpr Size capacity = Capacity;
  constexpr heap_capacity(Size) noexcept {};
};

template <typename Size, Size Capacity>
  requires (Capacity == dynamic_storage_capacity<Size>)
struct heap_capacity<Size, Capacity>
{
  Size capacity;
  constexpr heap_capacity(Size cap) noexcept : capacity {cap} {}
};

template <typename T, typename Size, Size Capacity, typename Allocator, exception_safety ExceptionSafety>
class heap_buffer_base
{
public:
  using size_type = Size;
  using difference_type = std::make_signed_t<size_type>;

  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<allocator_type>;

  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = allocator_traits::pointer;
  using const_pointer = allocator_traits::const_pointer;

private:
  static constexpr bool move_nothrow_ = std::is_nothrow_move_constructible_v<value_type>;

  static constexpr bool exception_safety_strong_ = ExceptionSafety == exception_safety::strong;

  using capacity_base_ = heap_capacity<size_type, Capacity>;
  static constexpr bool capacity_fixed_ = Capacity != dynamic_storage_capacity<size_type>;

  static constexpr bool allocator_fixed_ = allocator_traits::is_always_equal::value;
  static constexpr bool allocator_pocma_ = allocator_traits::propagate_on_container_move_assignment::value;
  static constexpr bool allocator_pocca_ = allocator_traits::propagate_on_container_copy_assignment::value;

  struct buffer_type_ : capacity_base_
  {
    [[no_unique_address]] allocator_type allocator;
    size_type size;
    pointer data;

    [[nodiscard]] constexpr reference operator [](size_type i) noexcept { return data[i]; }
    [[nodiscard]] constexpr const_reference operator [](size_type i) const noexcept { return data[i]; }

    constexpr void destroy(size_type count) noexcept pre(count <= size)
    {
      if constexpr (std::is_trivially_destructible_v<value_type>) {
        size -= count;
      } else {
        while (count > 0) {
          --count;
          allocator_traits::destroy(allocator, data + static_cast<difference_type>(--size));
        }
      }
    }

    constexpr void deallocate() noexcept
    {
      if (data) {
        allocator_traits::deallocate(allocator, data, this->capacity);
        data = nullptr;
      }
    }

    constexpr explicit buffer_type_(size_type cap, const allocator_type& alloc)
      : capacity_base_(cap), allocator(alloc), size {},
        data {this->capacity > 0 ? allocator_traits::allocate(allocator, this->capacity) : nullptr}
    {}

    constexpr buffer_type_(buffer_type_&& other) noexcept
      : capacity_base_(std::move(other)), allocator(std::move(other.allocator)), size {std::exchange(other.size, 0)},
        data {std::exchange(other.data, nullptr)}
    {}

    constexpr buffer_type_& operator =(buffer_type_&& other) noexcept
    {
      if (this == std::addressof(other))
        return *this;

      destroy(size);
      deallocate();
      capacity_base_::operator =(std::move(other));
      if constexpr (allocator_pocma_)
        allocator = std::move(other.allocator);
      size = std::exchange(other.size, 0);
      data = std::exchange(other.data, nullptr);
      return *this;
    }

    buffer_type_(const buffer_type_&) = delete;
    buffer_type_& operator =(const buffer_type_&) = delete;

    constexpr ~buffer_type_() noexcept
    {
      destroy(size);
      deallocate();
    }
  };

protected:
  buffer_type_ buffer_;

public:
  [[nodiscard]] constexpr size_type max_size() const noexcept { return allocator_traits::max_size(buffer_.allocator); }
  [[nodiscard]] constexpr size_type capacity() const noexcept { return buffer_.capacity; }

  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept { return buffer_.allocator; }

  [[nodiscard]] constexpr const_pointer data() const noexcept { return buffer_.data; }

  constexpr void clear() noexcept { buffer_.destroy(buffer_.size); }

  constexpr void recapacity(size_type count)
    requires (!capacity_fixed_)
  pre(count > buffer_.capacity)
  {
    if (buffer_.capacity == 0) {
      buffer_.data = allocator_traits::allocate(buffer_.allocator, count);
      buffer_.capacity = count;
      return;
    }

    const pointer ndata = allocator_traits::allocate(buffer_.allocator, count);
    try {
      if constexpr (exception_safety_strong_ && !move_nothrow_)
        uninitialized_copy_n(buffer_.allocator, buffer_.data, buffer_.size, ndata);
      else
        uninitialized_move_n(buffer_.allocator, buffer_.data, buffer_.size, ndata);
    } catch (...) {
      allocator_traits::deallocate(buffer_.allocator, ndata, count);
      throw;
    }
    const size_type nsize = buffer_.size;
    clear();
    buffer_.deallocate();
    buffer_.capacity = count;
    buffer_.size = nsize;
    buffer_.data = ndata;
  }

protected:
  constexpr explicit heap_buffer_base(const allocator_type& alloc) : buffer_(0, alloc) {}

  constexpr explicit heap_buffer_base(size_type cap, const allocator_type& alloc)
    requires (!capacity_fixed_)
    : buffer_(cap, alloc)
  {}

  constexpr heap_buffer_base(heap_buffer_base&& other) noexcept = default;

  constexpr heap_buffer_base& operator =(heap_buffer_base&& other) noexcept(allocator_fixed_ || allocator_pocma_)
  {
    if (this == std::addressof(other))
      return *this;

    if (allocator_fixed_ || allocator_pocma_ || buffer_.allocator == other.buffer_.allocator) {
      buffer_ = std::move(other.buffer_);
      return *this;
    }

    if ((move_nothrow_ || !exception_safety_strong_) && (buffer_.data && buffer_.capacity >= other.buffer_.size)) {
      buffer_.clear();
      uninitialized_move_n(buffer_.allocator, other.buffer_.data, other.buffer_.size, buffer_.data);
      buffer_.size = other.buffer_.size;
      return *this;
    }

    auto tmpbuf = buffer_type_(other.buffer_.size, buffer_.allocator);
    if constexpr (exception_safety_strong_ && !move_nothrow_)
      uninitialized_copy_n(buffer_.allocator, other.buffer_.data, other.buffer_.size, tmpbuf.data);
    else
      uninitialized_move_n(buffer_.allocator, other.buffer_.data, other.buffer_.size, tmpbuf.data);
    tmpbuf.size = other.buffer_.size;

    if constexpr (!capacity_fixed_)
      std::swap(buffer_.capacity, tmpbuf.capacity);
    std::swap(buffer_.size, tmpbuf.size);
    std::swap(buffer_.data, tmpbuf.data);
    return *this;
  }

  constexpr heap_buffer_base(const heap_buffer_base& other)
    : buffer_(other.buffer_.size, allocator_traits::select_on_container_copy_construction(other.buffer_.allocator))
  {
    uninitialized_copy_n(buffer_.allocator, other.buffer_.data, other.buffer_.size, buffer_.data);
    buffer_.size = other.buffer_.size;
  }

  constexpr auto& operator =(const heap_buffer_base& other)
  {
    if (this == std::addressof(other))
      return *this;

    if (
      exception_safety_strong_ || (allocator_pocca_ && buffer_.allocator != other.buffer_.allocator) || buffer_.data == nullptr
      || buffer_.capacity < other.buffer_.size
    ) {
      auto tmpbuf = buffer_type_(other.buffer_.size, allocator_pocca_ ? other.buffer_.allocator : buffer_.allocator);
      uninitialized_copy_n(tmpbuf.allocator, other.buffer_.data, other.buffer_.size, tmpbuf.data);
      tmpbuf.size = other.buffer_.size;

      if constexpr (!capacity_fixed_)
        std::swap(buffer_.capacity, tmpbuf.capacity);
      if constexpr (allocator_pocca_)
        std::swap(buffer_.allocator, tmpbuf.allocator);
      std::swap(buffer_.size, tmpbuf.size);
      std::swap(buffer_.data, tmpbuf.data);
      return *this;
    }

    buffer_.clear();
    uninitialized_copy_n(buffer_.allocator, other.buffer_.data, other.buffer_.size, buffer_.data);
    buffer_.size = other.buffer_.size;
    return *this;
  }
};

} // namespace fmia

export namespace fmia {

// contiguous heap memory whose size is determined at runtime
template <typename T, meta::size_integral Size, typename Allocator>
using relaxed_heap_buffer = heap_buffer_base<T, Size, dynamic_storage_capacity<Size>, Allocator, exception_safety::basic>;

template <typename T, meta::size_integral Size, typename Allocator>
using heap_buffer = heap_buffer_base<T, Size, dynamic_storage_capacity<Size>, Allocator, exception_safety::strong>;

// contiguous heap memory whose size is determined at compile-time
template <typename T, std::size_t Capacity, typename Allocator>
using relaxed_fixed_heap_buffer = heap_buffer_base<T, std::size_t, Capacity, Allocator, exception_safety::basic>;

template <typename T, std::size_t Capacity, typename Allocator>
using fixed_heap_buffer = heap_buffer_base<T, std::size_t, Capacity, Allocator, exception_safety::strong>;

} // export namespace fmia
