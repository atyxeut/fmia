// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import test.fmia.sort.comparison;

int main() {
  check_insertion_sort_correctness();
  check_selection_sort_correctness();
  check_bubble_sort_correctness();
  check_heap_sort_correctness();
  check_hoare_quick_sort_correctness();
  check_merge_sort_correctness();
}
