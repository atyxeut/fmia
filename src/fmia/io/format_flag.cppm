// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
