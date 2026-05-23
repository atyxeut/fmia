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
import fmia.memory.core;
import fmia.util.enum_flag;

export namespace fmia {

enum class storage_location : enum_underlying_type { heap, inplace };

} // export namespace fmia

namespace fmia {

template <meta::index_integral T>
constexpr T dynamic_capacity = -1;

template <typename Size, Size Capacity>
struct heap_capacity;

template <typename Size, Size Capacity>
  requires (Capacity > 0)
struct heap_capacity<Size, Capacity>
{
  static constexpr Size capacity = Capacity;
};

template <typename Size, Size Capacity>
  requires (Capacity == dynamic_capacity<Size>)
struct heap_capacity<Size, Capacity>
{
  Size capacity = 0;

  constexpr heap_capacity() noexcept = default;
  constexpr heap_capacity(Size cap) noexcept : capacity {cap} {}
  constexpr heap_capacity(heap_capacity&& other) noexcept : capacity {std::exchange(other.capacity, 0)} {}

  constexpr auto& operator =(heap_capacity&& other) noexcept
  {
    if (this == std::addressof(other))
      return *this;

    capacity = std::exchange(other.capacity, 0);
    return *this;
  }

  constexpr heap_capacity(const heap_capacity&) noexcept = default;
  constexpr auto& operator =(const heap_capacity&) noexcept = default;
};

template <typename T, typename Allocator, typename Size, Size Capacity, exception_safety ExceptionSafety>
class heap_storage_base
{
public:
  using size_type = Size;
  using difference_type = std::make_signed_t<Size>;

  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<allocator_type>;

  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = allocator_traits::pointer;
  using const_pointer = allocator_traits::const_pointer;

private:
  static constexpr bool capacity_fixed_ = Capacity != dynamic_capacity<size_type>;

  static constexpr bool allocator_fixed_ = allocator_traits::is_always_equal::value;
  static constexpr bool allocator_pocma_ = allocator_traits::propagate_on_container_move_assignment::value;
  static constexpr bool allocator_pocca_ = allocator_traits::propagate_on_container_copy_assignment::value;

  struct buffer_type_ : heap_capacity<Size, Capacity>
  {
    using capacity_base = heap_capacity<Size, Capacity>;

    [[no_unique_address]] allocator_type allocator;
    size_type size;
    pointer data;

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

    constexpr explicit buffer_type_(size_type cap, const allocator_type& alloc, size_type s = 0)
      : allocator(alloc), size {s}
    {
      if constexpr (capacity_fixed_)
        data = allocator_traits::allocate(allocator, Capacity);
      else
        data = cap > 0 ? allocator_traits::allocate(allocator, this->capacity = cap) : nullptr;
    }

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
  [[nodiscard]] constexpr auto capacity() const noexcept { return buffer_.capacity; }
  [[nodiscard]] constexpr auto size() const noexcept { return buffer_.size; }
  [[nodiscard]] consteval auto max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }
  [[nodiscard]] constexpr auto get_allocator() const noexcept { return buffer_.allocator; }
  [[nodiscard]] constexpr auto data() const noexcept { return buffer_.data; }

  constexpr void clear() noexcept { buffer_.clear(); }

protected:
  constexpr explicit heap_storage_base(const allocator_type& alloc = allocator_type {})
    requires (capacity_fixed_)
    : buffer_(0, alloc)
  {}

  constexpr explicit heap_storage_base(size_type cap, const allocator_type& alloc = allocator_type {})
    requires (!capacity_fixed_)
    : buffer_(cap, alloc)
  {}

  constexpr heap_storage_base(heap_storage_base&& other) noexcept = default;

  constexpr heap_storage_base& operator =(heap_storage_base&& other) noexcept(allocator_fixed_ || allocator_pocma_)
  {
    if (this == std::addressof(other))
      return *this;

    if (allocator_fixed_ || allocator_pocma_ || buffer_.allocator == other.buffer_.allocator) {
      buffer_ = std::move(other.buffer_);
      return *this;
    }

    if (
      (std::is_nothrow_move_constructible_v<value_type> || ExceptionSafety == exception_safety::basic)
      && (data() && capacity() >= other.size())
    ) {
      clear();
      for (; buffer_.size < other.size(); ++buffer_.size)
        allocator_traits::construct(buffer_.allocator, data() + buffer_.size, std::move(other.data()[buffer_.size]));
      return *this;
    }

    auto tmpbuf = buffer_type_(other.size(), buffer_.allocator);
    if constexpr (ExceptionSafety == exception_safety::strong && !std::is_nothrow_move_constructible_v<value_type>)
      for (; tmpbuf.size < other.size(); ++tmpbuf.size)
        allocator_traits::construct(buffer_.allocator, tmpbuf.data + tmpbuf.size, other.data()[tmpbuf.size]);
    else
      for (; tmpbuf.size < other.size(); ++tmpbuf.size)
        allocator_traits::construct(buffer_.allocator, tmpbuf.data + tmpbuf.size, std::move(other.data()[tmpbuf.size]));

    if constexpr (!capacity_fixed_)
      std::swap(buffer_.capacity, tmpbuf.capacity);
    std::swap(buffer_.size, tmpbuf.size);
    std::swap(buffer_.data, tmpbuf.data);
    return *this;
  }

  constexpr heap_storage_base(const heap_storage_base& other)
    : buffer_(other.size(), allocator_traits::select_on_container_copy_construction(other.buffer_.allocator))
  {
    for (; buffer_.size < other.size(); ++buffer_.size)
      allocator_traits::construct(buffer_.allocator, data() + buffer_.size, other.data()[buffer_.size]);
  }

  constexpr auto& operator =(const heap_storage_base& other)
  {
    if (this == std::addressof(other))
      return *this;

    if (
      ExceptionSafety == exception_safety::strong || (allocator_pocca_ && buffer_.allocator != other.buffer_.allocator)
      || data() == nullptr || capacity() < other.size()
    ) {
      auto tmpbuf = buffer_type_(other.size(), allocator_pocca_ ? other.buffer_.allocator : buffer_.allocator);
      for (; tmpbuf.size < other.size(); ++tmpbuf.size)
        allocator_traits::construct(tmpbuf.allocator, tmpbuf.data + tmpbuf.size, other.data()[tmpbuf.size]);

      if constexpr (!capacity_fixed_)
        std::swap(buffer_.capacity, tmpbuf.capacity);
      if constexpr (allocator_pocca_)
        std::swap(buffer_.allocator, tmpbuf.allocator);
      std::swap(buffer_.size, tmpbuf.size);
      std::swap(buffer_.data, tmpbuf.data);
      return *this;
    }

    clear();
    for (; buffer_.size < other.size(); ++buffer_.size)
      allocator_traits::construct(buffer_.allocator, buffer_.data + buffer_.size, other.data()[buffer_.size]);
    return *this;
  }
};

} // namespace fmia

export namespace fmia {

// the size of the heap memory is determined at runtime
template <typename T, meta::index_integral Size = usize, typename Allocator = std::allocator<T>>
using heap_storage = heap_storage_base<T, Allocator, Size, dynamic_capacity<Size>, exception_safety::basic>;

template <typename T, meta::index_integral Size = usize, typename Allocator = std::allocator<T>>
using safer_heap_storage = heap_storage_base<T, Allocator, Size, dynamic_capacity<Size>, exception_safety::strong>;

// the size of the heap memory is determined at compile-time
template <typename T, usize Capacity, typename Allocator = std::allocator<T>>
using fixed_heap_storage = heap_storage_base<T, Allocator, usize, Capacity, exception_safety::basic>;

template <typename T, usize Capacity, typename Allocator = std::allocator<T>>
using safer_fixed_heap_storage = heap_storage_base<T, Allocator, usize, Capacity, exception_safety::strong>;

} // export namespace fmia
