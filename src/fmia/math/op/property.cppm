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

export module fmia.op.property;

import std;

export namespace fmia::meta {

template <typename T>
inline constexpr bool enable_idempotent_operator = false;
  
template <typename Operator, typename T>
concept idempotent_operator = std::regular_invocable<Operator, T, T> 
&& std::convertible_to<std::invoke_result_t<Operator, T, T>, T> 
&& enable_idempotent_operator<std::remove_cvref_t<Operator>>;

} // export namespace fmia::meta

export namespace fmia {

template <typename Operator>
class idempotent_operator_wrapper final
{
private:
  Operator f_;
public:
  constexpr explicit idempotent_operator_wrapper(const Operator& f) : f_(f) {}
  constexpr explicit idempotent_operator_wrapper(Operator&& f) : f_(std::move(f)) {}

  template <typename... Args>
    requires std::regular_invocable<const Operator&, Args...>
  constexpr decltype(auto) operator()(Args&&... args) const noexcept(std::is_nothrow_invocable_v<const Operator&, Args...>) 
  {
    return std::invoke(f_, std::forward<Args>(args)...);
  }
};

template <typename Operator>
idempotent_operator_wrapper(Operator&& f) -> idempotent_operator_wrapper<std::remove_cvref_t<Operator>>;

template <typename Operator>
inline constexpr bool enable_idempotent_operator<idempotent_operator_wrapper<Operator>> = true;

} // export namespace fmia
