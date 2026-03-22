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

export module mzia.math;

export import mzia.math.equation;
export import mzia.math.floating_point;
export import mzia.math.fraction;
export import mzia.math.geometry;
export import mzia.math.integer;

export namespace mzia::meta {

template <typename T>
concept arithmetic = integral<T> || floating_point<T>;

} // namespace mzia::meta

