// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.string.character.ascii;

export namespace fmia {

[[nodiscard]] constexpr bool is_ascii_digit(char ch) noexcept
{
  return '0' <= ch & ch <= '9';
}

} // export namespace fmia
