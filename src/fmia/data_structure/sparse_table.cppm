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
  using difference_type = Buffer::difference_type;

  using allocator_type = Buffer::allocator_type;
  using allocator_traits = Buffer::allocator_traits;

private:
  [[no_unique_address]] MergeFn fn_;

  const size_type size_;
  const size_type max_exp_p1_;

  [[nodiscard]] constexpr size_type offset_(size_type i, size_type j) const noexcept
  {
    // the jth row has only row_length - 2^j + 1 elements, so the jth row has 2^j - 1 unused slots, then from 0th row to the (j-1)th, the
    // total contribution is sum_(k = 0)^(j - 1) (2^k - 1) = 2^j - j - 1 spare slots, thus the offset is j * row_length + i - (2^j - j - 1)
    //
    // by the way, the total memory saved for the whole table that has max jump J is (2^J - J - 1) + (2^J - 1) = 2^(J + 1) - J - 2 slots,
    // i.e. saved O(n) for n elements
    return j * size_ + i + j + 1 - (static_cast<size_type>(1) << j);
  }

public:
  [[nodiscard]] constexpr auto&& operator [](this auto&& self, size_type i, size_type j) noexcept //
    pre(self.offset_(i, j) < self.buffer_.size)
  {
    return std::forward<decltype(self)>(self).buffer_[self.offset_(i, j)];
  }

  [[nodiscard]] constexpr T query(
    size_type l, size_type r
  ) const noexcept(std::is_nothrow_invocable_v<const MergeFn&, const T&, const T&>) //
    pre(l <= r)
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
  constexpr explicit sparse_table(R&& r, F&& fn = MergeFn {}, const allocator_type& alloc = allocator_type {}) //
    pre(std::ranges::size(r) > 0)
    : Buffer(alloc), fn_(std::forward<F>(fn)), size_ {std::ranges::size(r)}, max_exp_p1_ {static_cast<size_type>(ilog2(size_) + 1)}
  {
    // sum_(j = 0)^(J) (n - 2^j + 1) = (n + 1)(j + 1) - (2^(j + 1) - 1)
    this->recapacity((size_ + 1) * max_exp_p1_ - (static_cast<size_type>(1) << max_exp_p1_) + 1);
    if constexpr (std::is_rvalue_reference_v<R&&> || std::is_rvalue_reference_v<std::ranges::range_reference_t<R>>)
      uninitialized_move_n(this->buffer_.allocator, std::ranges::begin(r), size_, this->buffer_.data);
    else
      uninitialized_copy_n(this->buffer_.allocator, std::ranges::begin(r), size_, this->buffer_.data);
    this->buffer_.size = size_;

    for (size_type j = 1; j < max_exp_p1_; ++j) {
      const auto len = static_cast<size_type>(1) << j;
      for (size_type i = 0; i + len <= size_; ++i) {
        allocator_traits::construct(
          this->buffer_.allocator, this->buffer_.data + static_cast<difference_type>(offset_(i, j)),
          std::invoke(fn_, (*this)[i, j - 1], (*this)[i + (len >> 1), j - 1])
        );
        ++this->buffer_.size;
      }
    }
  }
};

template <std::ranges::forward_range R, typename F, typename Allocator = std::allocator<std::ranges::range_value_t<R>>>
sparse_table(R&&, F&&, Allocator = Allocator {}) -> sparse_table<
  std::ranges::range_value_t<R>, std::remove_reference_t<F>, relaxed_heap_buffer<std::ranges::range_value_t<R>, std::size_t, Allocator>
>;

} // export namespace fmia
