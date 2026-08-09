// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import test.fmia.sort.internal;

int main() {
  check_insertion_sort();
  check_binary_insertion_sort();
  check_selection_sort();
  check_bubble_sort();
  check_heap_sort();
  check_recursive_hoare_quick_sort();
  check_recursive_hoare_quick_sort_less_comparison();
  check_merge_sort();
}
