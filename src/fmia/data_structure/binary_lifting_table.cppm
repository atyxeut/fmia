// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.binary_lifting_table;

import std;

import fmia.math.integer.fixed_precision;
import fmia.memory.allocator;
import fmia.memory.storage_base;

export namespace fmia {

template <typename T, typename Merge, typename Project, typename I = std::size_t>
struct binary_lifting_policy_base
{
  using value_type = T;
  using merge_type = Merge;
  using project_type = Project;
  using index_type = I;
};

} // export namespace fmia

export namespace fmia::meta {

template <typename Policy>
concept binary_lifting_policy = requires(typename Policy::value_type a, typename Policy::value_type b) {
  typename Policy::merge_type;
  typename Policy::project_type;
  typename Policy::index_type;

  { Policy::merge(a, b) } -> std::convertible_to<typename Policy::value_type>;
  { Policy::sentinel_value } -> std::same_as<typename Policy::value_type>;

  { Policy::project(a) } -> std::same_as<typename Policy::index_type>;
  { Policy::sentinel_index } -> std::same_as<typename Policy::index_type>;
};

} // export namespace fmia::meta

export namespace fmia {

template <typename T, meta::binary_lifting_policy Policy, typename Buffer = relaxed_heap_buffer<T, std::size_t, std::allocator<T>>>
  requires std::same_as<T, typename Policy::value_type>
class binary_lifting_table final : private Policy, public Buffer
{
public:
  using merge_type = Policy::merge_type;
  using project_type = Policy::project_type;

  using size_type = Buffer::size_type;
  using difference_type = Buffer::difference_type;

  using allocator_type = Buffer::allocator_type;
  using allocator_traits = Buffer::allocator_traits;

private:
  [[no_unique_address]] merge_type merge_;
  [[no_unique_address]] project_type project_;

  const size_type size_;
  const size_type max_exp_p1_;

private:
  [[nodiscard]] constexpr size_type map_(size_type i, size_type j) const noexcept { return i * max_exp_p1_ + j; }

public:
  template <std::ranges::forward_range R, typename P = Policy>
    requires std::same_as<Policy, std::remove_cvref_t<P>>
  constexpr explicit binary_lifting_table(R&& r, P&& p, const allocator_type& alloc = allocator_type {}) //
    pre(std::ranges::distance(r) > 0)
    : Policy(std::forward<P>(p)), Buffer(alloc), size_ {static_cast<size_type>(std::ranges::distance(r))},
      max_exp_p1_ {static_cast<size_type>(ilog2(size_) + 1)}
  {
    this->recapacity(size_ * max_exp_p1_);

    size_type row = 0;
    size_type cur_i = 0;
    size_type cur_j = 1;
    try {
      for (auto&& item : r) {
        if constexpr (!std::is_lvalue_reference_v<R&&> || !std::is_lvalue_reference_v<std::ranges::range_reference_t<R>>)
          allocator_traits::construct(this->buffer_.allocator, this->offset(map_(row, 0)), std::move(item));
        else
          allocator_traits::construct(this->buffer_.allocator, this->offset(map_(row, 0)), item);
        ++row;
      }
      for (; cur_j < max_exp_p1_; ++cur_j) {
        for (cur_i = 0; cur_i < size_; ++cur_i) {
          auto& mid = this->buffer_[map_(cur_i, cur_j - 1)];
          const size_type mid_idx = static_cast<size_type>(std::invoke(project_, mid));
          if (const auto ptr = this->offset(map_(cur_i, cur_j)); mid_idx == Policy::sentinel_index)
            allocator_traits::construct(this->buffer_.allocator, ptr, Policy::sentinel_value);
          else
            allocator_traits::construct(this->buffer_.allocator, ptr, std::invoke(merge_, mid, this->buffer_[map_(mid_idx, cur_j - 1)]));
        }
      }
      this->buffer_.size = max_exp_p1_ * size_;
    } catch (...) {
      while (cur_i > 0)
        allocator_traits::destroy(this->buffer_.allocator, this->offset(map_(--cur_i, cur_j)));
      while (cur_j > 1) {
        --cur_j;
        for (size_type i = size_; i > 0;)
          allocator_traits::destroy(this->buffer_.allocator, this->offset(map_(--i, cur_j)));
      }
      while (row > 0)
        allocator_traits::destroy(this->buffer_.allocator, this->offset(map_(--row, 0)));
      throw;
    }
  }

public:
  [[nodiscard]] constexpr auto query(size_type i, size_type distance, T identity_element) const noexcept(
    noexcept(std::is_nothrow_invocable_v<merge_type, T, T> && std::is_nothrow_invocable_v<project_type, T>)
  )
  {
    T result = std::move(identity_element);
    for (size_type j = max_exp_p1_; j > 0;) {
      --j;
      if (distance >> j & 1) {
        result = std::invoke(merge_, result, this->buffer_[map_(i, j)]);
        i = std::invoke(project_, this->buffer_[map_(i, j)]);
      }
    }
    return result;
  }
};

template <std::ranges::forward_range R, typename Policy, typename Allocator = std::allocator<std::ranges::range_value_t<R>>>
binary_lifting_table(R&&, Policy&&, const Allocator& = Allocator {}) -> binary_lifting_table<
  std::ranges::range_value_t<R>, std::remove_reference_t<Policy>, relaxed_heap_buffer<std::ranges::range_value_t<R>, std::size_t, Allocator>
>;

} // export namespace fmia
