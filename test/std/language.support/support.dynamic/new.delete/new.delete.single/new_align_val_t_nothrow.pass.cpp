//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// Aligned allocation was not provided before macosx10.14 and as a result we
// get availability errors when the deployment target is older than macosx10.14.
// However, support for that was broken prior to Clang 8 and AppleClang 11.
// UNSUPPORTED: apple-clang-9, apple-clang-10
// UNSUPPORTED: clang-5, clang-6, clang-7
// XFAIL: with_system_cxx_lib=macosx10.13
// XFAIL: with_system_cxx_lib=macosx10.12
// XFAIL: with_system_cxx_lib=macosx10.11
// XFAIL: with_system_cxx_lib=macosx10.10
// XFAIL: with_system_cxx_lib=macosx10.9

// asan and msan will not call the new handler.
// UNSUPPORTED: sanitizer-new-delete

// test operator new (nothrow)

#include <new>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <limits>

#include "test_macros.h"

constexpr auto OverAligned = __STDCPP_DEFAULT_NEW_ALIGNMENT__ * 2;

int new_handler_called = 0;

void my_new_handler()
{
    ++new_handler_called;
    std::set_new_handler(0);
}

bool A_constructed = false;

struct alignas(OverAligned) A
{
    A() {A_constructed = true;}
    ~A() {A_constructed = false;}
};

void test_max_alloc() {
    std::set_new_handler(my_new_handler);
    auto do_test = []() {
        void* vp = operator new (std::numeric_limits<std::size_t>::max(),
                                 std::align_val_t(OverAligned),
                                 std::nothrow);
        assert(new_handler_called == 1);
        assert(vp == 0);
    };
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        do_test();
    }
    catch (...)
    {
        assert(false);
    }
#else
    do_test();
#endif
}

int main(int, char**)
{
    {
        A* ap = new(std::nothrow) A;
        assert(ap);
#ifdef __CHERI_PURE_CAPABILITY__
        assert((unsigned)reinterpret_cast<std::uintptr_t>(ap) % OverAligned == 0);
#else
        assert(reinterpret_cast<std::uintptr_t>(ap) % OverAligned == 0);
#endif
        assert(A_constructed);
        delete ap;
        assert(!A_constructed);
    }
    {
        test_max_alloc();
    }

  return 0;
}
