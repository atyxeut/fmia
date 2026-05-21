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

export module fmia.io.format_flag;

import std;

import fmia.util.enum_flag;

export namespace fmia::io {

enum class fmt : enum_flag_underlying_type { none, endl };

} // export namespace fmia::io

export namespace fmia::meta {

template <>
struct is_enum_flag<io::fmt> : std::true_type
{
};

} // export namespace fmia::meta
