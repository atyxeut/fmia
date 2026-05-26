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

module;

#include <cassert>

export module fmia.data_structure.sparse_table;

import std;

import fmia.math.integer.fixed_precision;
import fmia.memory.core;
import fmia.memory.storage_base;

namespace fmia {

template <typename T, meta::index_integral Size, typename Allocator>
class sparse_table_base : heap_buffer<T, Size, Allocator>
{
private:
  using buffer_base_ = heap_buffer<T, Size, Allocator>;

public:
  using size_type = buffer_base_::size_type;
  using difference_type = buffer_base_::difference_type;

  using allocator_type = buffer_base_::Allocator;

  using value_type = buffer_base_::value_type;
  using reference = buffer_base_::reference;
  using const_reference = buffer_base_::const_reference;
  using pointer = buffer_base_::pointer;
  using const_pointer = buffer_base_::const_pointer;

private:
  const size_type size_;
  const size_type max_jump_;

public:
  [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

  [[nodiscard]] constexpr reference operator [](Size i, Size j) noexcept
  {
    assert(i * size_ + j < this->size() && "subscript out of range\n");
    return this->buffer_[i * size_ + j];
  }

  [[nodiscard]] constexpr const_reference operator [](Size i, Size j) const noexcept
  {
    assert(i * size_ + j < this->size() && "subscript out of range\n");
    return this->buffer_[i * size_ + j];
  }
};

} // namespace fmia
