// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.io.buffered_io;

import std;

import fmia.memory;

export namespace fmia {

template <std::size_t = 16 * 1024 * 1024, storage_location = storage_location::inplace>
class buffered_reader;

template <std::size_t = 16 * 1024 * 1024, storage_location = storage_location::inplace>
class buffered_writer;

template <std::size_t BufferSize>
class buffered_writer<BufferSize, storage_location::inplace> {
  void flush() {}

  ~buffered_writer() { flush(); }
};

template <std::size_t BufferSize>
class buffered_writer<BufferSize, storage_location::heap> {
  void flush() {}

  ~buffered_writer() { flush(); }
};

} // export namespace fmia
