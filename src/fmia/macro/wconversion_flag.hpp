// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

// clang-format off

#ifdef _MSC_VER
  #define FMIA_WCONVERSION_PUSH() __pragma(warning(push))          \
                                  __pragma(warning(disable: 4018)) \
                                  __pragma(warning(disable: 4244)) \
                                  __pragma(warning(disable: 4267)) \
                                  __pragma(warning(disable: 4389))
  #define FMIA_WCONVERSION_POP() __pragma(warning(pop))
#elifdef __clang__
  #define FMIA_WCONVERSION_PUSH() _Pragma("clang diagnostic push")                          \
                                  _Pragma("clang diagnostic ignored \"-Wsign-conversion\"") \
                                  _Pragma("clang diagnostic ignored \"-Wsign-compare\"")    \
                                  _Pragma("clang diagnostic ignored \"-Wconversion\"")
  #define FMIA_WCONVERSION_POP() _Pragma("clang diagnostic pop")
#elifdef __GNUC__
  #define FMIA_WCONVERSION_PUSH() _Pragma("GCC diagnostic push")                          \
                                  _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"") \
                                  _Pragma("GCC diagnostic ignored \"-Wsign-compare\"")    \
                                  _Pragma("GCC diagnostic ignored \"-Wconversion\"")
  #define FMIA_WCONVERSION_POP() _Pragma("GCC diagnostic pop")
#else
  #define FMIA_WCONVERSION_PUSH()
  #define FMIA_WCONVERSION_POP()
#endif

// clang-format on
