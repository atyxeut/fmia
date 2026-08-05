// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import test.fmia.io.range_output;

int main() {
  does_cstyle_string_literal_output_not_trigger_ambiguous_overloads();
  check_common_ranges_output_with_custom_print_and_std_ostream();
  check_multidimensional_ranges_output_with_custom_print_and_std_ostream();
}
