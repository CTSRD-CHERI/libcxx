//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <functional>

// class function<R(ArgTypes...)>

// function& operator=(nullptr_t);

#include <functional>
#include <cassert>

#include "test_macros.h"

struct A
{
  static std::function<void()> global;
  static bool cancel;

  ~A() {
    DoNotOptimize(cancel);
    if (cancel)
      global = nullptr;
  }
  void operator()() {}
};

std::function<void()> A::global;
bool A::cancel = false;

int main()
{
  A::global = A();
#ifndef TEST_HAS_NO_RTTI
  assert(A::global.target<A>());
#endif

  // Check that we don't recurse in A::~A().
  A::cancel = true;
  A::global = nullptr;

#ifndef TEST_HAS_NO_RTTI
  assert(!A::global.target<A>());
#endif
}
