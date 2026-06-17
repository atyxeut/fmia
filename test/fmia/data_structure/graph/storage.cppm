// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.data_structure.graph.storage;

import std;

import fmia.data_structure.graph.storage;

using namespace fmia;

export {

void edge_class_initialization()
{
  graph::undirected_edge<std::size_t, std::size_t, int, std::size_t, std::size_t> edge {1, 2, 3, 4};
}

} // export
