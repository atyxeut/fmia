// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.io.format_flag;

import std;

import fmia.meta.enum_flag;

export namespace fmia {

enum class iofmt : std::uint32_t { none = 0, endl = 1 };

} // export namespace fmia

export namespace fmia::meta {

template <>
struct is_enum_flag<iofmt> : std::true_type {};

} // export namespace fmia::meta
