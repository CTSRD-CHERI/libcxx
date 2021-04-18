//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// Compare iterators from different containers with <.

// UNSUPPORTED: libcxx-no-debug-mode

// ADDITIONAL_COMPILE_FLAGS: -D_LIBCPP_DEBUG=1

#include <vector>

#include "test_macros.h"
#include "debug_macros.h"
#include "min_allocator.h"

int main(int, char**) {
  typedef int T;
  typedef std::vector<T, min_allocator<T> > C;
  C c1;
  C c2;
  TEST_LIBCPP_ASSERT_FAILURE(c1.begin() < c2.begin(), "Attempted to compare incomparable iterators");

  return 0;
}
