// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.memory.allocator;

export namespace fmia {

// represent std::allocator<T>
struct std_allocator_tag
{
};

// represent std::pmr::polymorphic_allocator<T>
struct std_pmr_allocator_tag
{
};

} // export namespace fmia
