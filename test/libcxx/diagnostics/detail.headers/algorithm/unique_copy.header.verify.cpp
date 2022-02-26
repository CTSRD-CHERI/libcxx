//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: modules-build
// UNSUPPORTED: clang-11, clang-12, clang-13
// UNSUPPORTED: apple-clang-11, apple-clang-12, apple-clang-13
// UNSUPPORTED: gcc-11
// UNSUPPORTED: libcpp-has-no-localization, libcpp-has-no-threads
// ADDITIONAL_COMPILE_FLAGS: -U_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER

// WARNING: This test was generated by 'generate_private_header_tests.py'
// and should not be edited manually.

#include <__algorithm/unique_copy.h>
// expected-error@-1 {{header '<__algorithm/unique_copy.h>' is an implementation detail; #include '<algorithm>' instead}}
