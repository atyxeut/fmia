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

export module fmia.data_structure.sparse_table;

import std;

import fmia.math.integer.fixed_precision;
import fmia.memory.storage_base;
import fmia.op.property;

export namespace fmia {

template <typename T, typename MergeFn, typename Buffer = relaxed_heap_buffer<T, std::size_t, std::allocator<T>>>
class sparse_table final : public Buffer
{
public:
  using size_type = Buffer::size_type;

  using allocator_type = Buffer::allocator_type;

  using reference = Buffer::reference;
  using const_reference = Buffer::const_reference;

private:
  [[no_unique_address]] MergeFn fn_;

  const size_type size_;
  const size_type max_exp_p1_;

public:
  [[nodiscard]] constexpr auto&& operator [](this auto&& self, size_type i, size_type j) noexcept pre(j* self.size_ + i < self.buffer_.size)
  {
    return std::forward<decltype(self)>(self).buffer_[j * self.size_ + i];
  }

  [[nodiscard]] constexpr T query(size_type l, size_type r) const noexcept(std::is_nothrow_invocable_v<const MergeFn&, T, T>) pre(l <= r)
  {
    if constexpr (meta::idempotent_operator<MergeFn, T>) {
      const size_type j = ilog2(r - l + 1);
      return std::invoke(fn_, (*this)[l, j], (*this)[r + 1 - (static_cast<size_type>(1) << j), j]);
    } else {
      const size_type d = r - l + 1;
      size_type j = ilog2(d);

      T res = (*this)[l, j];
      l += static_cast<size_type>(1) << j;
      while (l <= r) {
        --j;
        if (d >> j & 1) {
          res = std::invoke(fn_, std::move(res), (*this)[l, j]);
          l += static_cast<size_type>(1) << j;
        }
      }

      return res;
    }
  }

public:
  template <std::ranges::forward_range R, typename F = MergeFn>
  constexpr explicit sparse_table(R&& r, F&& fn = MergeFn {}, const allocator_type& alloc = allocator_type {})
    : Buffer(alloc), fn_(std::forward<F>(fn)), size_ {std::ranges::size(r)}, max_exp_p1_ {ilog2(size_) + 1}
  {
    this->recapacity(size_ * max_exp_p1_);

    if constexpr (std::is_rvalue_reference_v<R&&>)
      std::ranges::move(r, this->buffer_.data);
    else
      std::ranges::copy(r, this->buffer_.data);

    for (size_type j = 1; j < max_exp_p1_; ++j) {
      for (size_type i = 0, len = static_cast<size_type>(1) << j; i + len <= size_; ++i) {
        (*this)[i, j] = std::invoke(fn_, (*this)[i, j - 1], (*this)[i + (len >> 1), j - 1]);
      }
    }
  }
};

template <std::ranges::forward_range R, typename F, typename Allocator = std::allocator<std::ranges::range_value_t<R>>>
sparse_table(R&&, F&&, Allocator = Allocator {}) -> sparse_table<
  std::ranges::range_value_t<R>, std::remove_reference_t<F>, relaxed_heap_buffer<std::ranges::range_value_t<R>, std::size_t, Allocator>
>;

} // export namespace fmia
