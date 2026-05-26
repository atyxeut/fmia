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
import fmia.memory.allocator;
import fmia.memory.core;
import fmia.util.enum_flag;

export namespace fmia {

enum class storage_location : enum_underlying_type { heap, inplace };

} // export namespace fmia

namespace fmia {

template <meta::index_integral T>
constexpr T dynamic_storage_capacity = -1;

template <typename Size, Size Capacity>
struct heap_capacity;

template <typename Size, Size Capacity>
  requires (Capacity >= 0)
struct heap_capacity<Size, Capacity>
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

template <typename T, typename Allocator, typename Size, Size Capacity, exception_safety ExceptionSafety>
class heap_buffer_base
{
public:
  using size_type = Size;
  using difference_type = size_type;

  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<allocator_type>;

  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = allocator_traits::pointer;
  using const_pointer = allocator_traits::const_pointer;

private:
  static constexpr bool capacity_fixed_ = Capacity != dynamic_storage_capacity<size_type>;

  static constexpr bool allocator_fixed_ = allocator_traits::is_always_equal::value;
  static constexpr bool allocator_pocma_ = allocator_traits::propagate_on_container_move_assignment::value;
  static constexpr bool allocator_pocca_ = allocator_traits::propagate_on_container_copy_assignment::value;

  struct buffer_type_ : heap_capacity<Size, Capacity>
  {
    using capacity_base = heap_capacity<Size, Capacity>;

    [[no_unique_address]] allocator_type allocator;
    size_type size;
    pointer data;

    [[nodiscard]] constexpr reference operator [](size_type i) noexcept { return data[i]; }
    [[nodiscard]] constexpr const_reference operator [](size_type i) const noexcept { return data[i]; }

    constexpr void clear() noexcept
    {
      if constexpr (std::is_trivially_destructible_v<value_type>)
        size = 0;
      else
        while (size > 0)
          allocator_traits::destroy(allocator, data + --size);
    }

    constexpr void deallocate() noexcept
    {
      if (data) {
        allocator_traits::deallocate(allocator, data, this->capacity);
        data = nullptr;
      }
    }

    constexpr explicit buffer_type_(size_type cap, const allocator_type& alloc)
      : capacity_base(cap), allocator(alloc), size {},
        data {this->capacity > 0 ? allocator_traits::allocate(allocator, this->capacity) : nullptr}
    {}

    constexpr buffer_type_(buffer_type_&& other) noexcept
      : capacity_base(std::move(other)), allocator(std::move(other.allocator)), size {std::exchange(other.size, 0)},
        data {std::exchange(other.data, nullptr)}
    {}

    constexpr buffer_type_& operator =(buffer_type_&& other) noexcept
    {
      if (this == std::addressof(other))
        return *this;

      clear();
      deallocate();
      capacity_base::operator =(std::move(other));
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
      clear();
      deallocate();
    }
  };

protected:
  buffer_type_ buffer_;

public:
  [[nodiscard]] constexpr size_type max_size() const noexcept { return allocator_traits::max_size(buffer_.allocator); }
  [[nodiscard]] constexpr size_type capacity() const noexcept { return buffer_.capacity; }
  [[nodiscard]] constexpr size_type size() const noexcept { return buffer_.size; }

  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept { return buffer_.allocator; }

  [[nodiscard]] constexpr const_pointer data() const noexcept { return buffer_.data; }

  constexpr void clear() noexcept { buffer_.clear(); }

protected:
  constexpr explicit heap_buffer_base(const allocator_type& alloc = allocator_type {}) : buffer_(0, alloc) {}

  constexpr explicit heap_buffer_base(size_type cap, const allocator_type& alloc = allocator_type {})
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

    if (
      (std::is_nothrow_move_constructible_v<value_type> || ExceptionSafety == exception_safety::basic)
      && (buffer_.data && buffer_.capacity >= other.buffer_.size)
    ) {
      buffer_.clear();
      uninitialized_move_n(buffer_.allocator, other.buffer_.data, other.buffer_.size, buffer_.data);
      buffer_.size = other.buffer_.size;
      return *this;
    }

    auto tmpbuf = buffer_type_(other.buffer_.size, buffer_.allocator);
    if constexpr (ExceptionSafety == exception_safety::strong && !std::is_nothrow_move_constructible_v<value_type>)
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
      ExceptionSafety == exception_safety::strong || (allocator_pocca_ && buffer_.allocator != other.buffer_.allocator)
      || buffer_.data == nullptr || buffer_.capacity < other.buffer_.size
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
template <typename T, meta::index_integral Size, typename Allocator>
using heap_buffer = heap_buffer_base<T, Allocator, Size, dynamic_storage_capacity<Size>, exception_safety::basic>;

template <typename T, meta::index_integral Size, typename Allocator>
using safer_heap_buffer =
  heap_buffer_base<T, Allocator, Size, dynamic_storage_capacity<Size>, exception_safety::strong>;

// contiguous heap memory whose size is determined at compile-time
template <typename T, idx64 Capacity, typename Allocator>
using fixed_heap_buffer = heap_buffer_base<T, Allocator, idx64, Capacity, exception_safety::basic>;

template <typename T, idx64 Capacity, typename Allocator>
using safer_fixed_heap_buffer = heap_buffer_base<T, Allocator, idx64, Capacity, exception_safety::strong>;

} // export namespace fmia
