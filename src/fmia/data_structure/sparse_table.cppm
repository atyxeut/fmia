// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.sparse_table;

import std;

import fmia.math.algebra;
import fmia.math.integer.fixed_precision;
import fmia.memory.allocator;
import fmia.memory.storage_base;

export namespace fmia {

template <typename T, typename Merge, typename Buffer = relaxed_heap_buffer<T, std::size_t, std::allocator<T>>>
  requires std::regular_invocable<Merge, T, T>
class sparse_table final : public Buffer
{
public:
  using size_type = Buffer::size_type;
  using difference_type = Buffer::difference_type;

  using allocator_type = Buffer::allocator_type;
  using allocator_traits = Buffer::allocator_traits;

private:
  [[no_unique_address]] Merge f_;

  const size_type size_;
  const size_type max_exp_p1_;

private:
  [[nodiscard]] constexpr size_type map_(size_type i, size_type j) const noexcept
    // post(idx : idx < this->buffer_.size)
  {
    // the jth row has only row_length - 2^j + 1 elements, so the jth row has 2^j - 1 unused slots, then from 0th row to the (j-1)th, the
    // total contribution is sum_(k = 0)^(j - 1) (2^k - 1) = 2^j - j - 1 spare slots, thus the offset is j * row_length + i - (2^j - j - 1)
    //
    // by the way, the total memory saved for the whole table that has max jump J is (2^J - J - 1) + (2^J - 1) = 2^(J + 1) - J - 2 slots,
    // i.e. saved O(n) for n elements
    return j * size_ + i + j + 1 - (static_cast<size_type>(1) << j);
  }

public:
  template <std::ranges::forward_range R, typename F = Merge>
    requires std::same_as<Merge, std::remove_cvref_t<F>>
  constexpr explicit sparse_table(R&& r, F&& f = Merge {}, const allocator_type& alloc = allocator_type {}) //
    pre(std::ranges::distance(r) > 0)
    : Buffer(alloc), f_(std::forward<F>(f)), size_ {static_cast<size_type>(std::ranges::distance(r))},
      max_exp_p1_ {static_cast<size_type>(ilog2(size_) + 1)}
  {
    // sum_(j = 0)^(J) (n - 2^j + 1) = (n + 1)(j + 1) - (2^(j + 1) - 1)
    this->recapacity((size_ + 1) * max_exp_p1_ + 1 - (static_cast<size_type>(1) << max_exp_p1_));

    if constexpr (!std::is_lvalue_reference_v<R&&> || !std::is_lvalue_reference_v<std::ranges::range_reference_t<R>>)
      uninitialized_move_n(this->buffer_.allocator, std::ranges::begin(r), size_, this->buffer_.data);
    else
      uninitialized_copy_n(this->buffer_.allocator, std::ranges::begin(r), size_, this->buffer_.data);
    this->buffer_.size = size_;
    for (size_type j = 1; j < max_exp_p1_; ++j) {
      const size_type len = static_cast<size_type>(1) << j;
      for (size_type i = 0; i + len <= size_; ++i) {
        allocator_traits::construct(
          this->buffer_.allocator, this->offset(map_(i, j)),
          std::invoke(f_, this->buffer_[map_(i, j - 1)], this->buffer_[map_(i + (len >> 1), j - 1)])
        );
        ++this->buffer_.size;
      }
    }
  }

public:
  [[nodiscard]] constexpr T query(size_type l, size_type r) const noexcept(std::is_nothrow_invocable_v<const Merge&, const T&, const T&>) //
    pre(l <= r)
  {
    if constexpr (meta::idempotent_operator<T, Merge>) {
      const size_type j = ilog2(r - l + 1);
      return std::invoke(f_, this->buffer_[map_(l, j)], this->buffer_[map_(r + 1 - (static_cast<size_type>(1) << j), j)]);
    } else {
      const size_type d = r - l + 1;
      size_type j = ilog2(d);
      T res = this->buffer_[map_(l, j)];
      l += static_cast<size_type>(1) << j;
      while (l <= r) {
        --j;
        if (d >> j & 1) {
          res = std::invoke(f_, std::move(res), this->buffer_[map_(l, j)]);
          l += static_cast<size_type>(1) << j;
        }
      }
      return res;
    }
  }
};

template <std::ranges::forward_range R, typename F, typename Allocator = std::allocator<std::ranges::range_value_t<R>>>
sparse_table(R&&, F&&, const Allocator& = Allocator {}) -> sparse_table<
  std::ranges::range_value_t<R>, std::remove_reference_t<F>, relaxed_heap_buffer<std::ranges::range_value_t<R>, std::size_t, Allocator>
>;

} // export namespace fmia
