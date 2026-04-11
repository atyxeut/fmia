#pragma once

// clang-format off

#if defined(_MSC_VER)
  #define FMIA_WCONVERSION_PUSH() __pragma(warning(push))          \
                                  __pragma(warning(disable: 4018)) \
                                  __pragma(warning(disable: 4267)) \
                                  __pragma(warning(disable: 4389))

  #define FMIA_WCONVERSION_POP() __pragma(warning(pop))
#elif defined(__GNUC__) || defined (__clang__)
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
