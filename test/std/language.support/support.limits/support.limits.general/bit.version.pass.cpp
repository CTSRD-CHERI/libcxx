//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// WARNING: This test was generated by generate_feature_test_macro_components.py
// and should not be edited manually.

// <bit>

// Test the feature test macros defined by <bit>

/*  Constant              Value
    __cpp_lib_bit_cast    201806L [C++20]
    __cpp_lib_endian      201907L [C++20]
    __cpp_lib_int_pow2    202002L [C++20]
*/

#include <bit>
#include "test_macros.h"

#if TEST_STD_VER < 14

# ifdef __cpp_lib_bit_cast
#   error "__cpp_lib_bit_cast should not be defined before c++20"
# endif

# ifdef __cpp_lib_endian
#   error "__cpp_lib_endian should not be defined before c++20"
# endif

# ifdef __cpp_lib_int_pow2
#   error "__cpp_lib_int_pow2 should not be defined before c++20"
# endif

#elif TEST_STD_VER == 14

# ifdef __cpp_lib_bit_cast
#   error "__cpp_lib_bit_cast should not be defined before c++20"
# endif

# ifdef __cpp_lib_endian
#   error "__cpp_lib_endian should not be defined before c++20"
# endif

# ifdef __cpp_lib_int_pow2
#   error "__cpp_lib_int_pow2 should not be defined before c++20"
# endif

#elif TEST_STD_VER == 17

# ifdef __cpp_lib_bit_cast
#   error "__cpp_lib_bit_cast should not be defined before c++20"
# endif

# ifdef __cpp_lib_endian
#   error "__cpp_lib_endian should not be defined before c++20"
# endif

# ifdef __cpp_lib_int_pow2
#   error "__cpp_lib_int_pow2 should not be defined before c++20"
# endif

#elif TEST_STD_VER == 20

# if !defined(_LIBCPP_VERSION)
#   ifndef __cpp_lib_bit_cast
#     error "__cpp_lib_bit_cast should be defined in c++20"
#   endif
#   if __cpp_lib_bit_cast != 201806L
#     error "__cpp_lib_bit_cast should have the value 201806L in c++20"
#   endif
# else // _LIBCPP_VERSION
#   ifdef __cpp_lib_bit_cast
#     error "__cpp_lib_bit_cast should not be defined because it is unimplemented in libc++!"
#   endif
# endif

# ifndef __cpp_lib_endian
#   error "__cpp_lib_endian should be defined in c++20"
# endif
# if __cpp_lib_endian != 201907L
#   error "__cpp_lib_endian should have the value 201907L in c++20"
# endif

# ifndef __cpp_lib_int_pow2
#   error "__cpp_lib_int_pow2 should be defined in c++20"
# endif
# if __cpp_lib_int_pow2 != 202002L
#   error "__cpp_lib_int_pow2 should have the value 202002L in c++20"
# endif

#endif // TEST_STD_VER == 20

int main(int, char**) { return 0; }
