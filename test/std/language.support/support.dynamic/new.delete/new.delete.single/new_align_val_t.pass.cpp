//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// Aligned allocation was not provided before macosx10.13 and as a result we
// get availability errors when the deployment target is older than macosx10.13.
// XFAIL: use_system_cxx_lib && target={{.+}}-apple-macosx10.{{9|10|11|12}}

// asan and msan will not call the new handler.
// UNSUPPORTED: sanitizer-new-delete

// Libcxx when built for z/OS doesn't contain the aligned allocation functions,
// nor does the dynamic library shipped with z/OS.
// UNSUPPORTED: target={{.+}}-zos{{.*}}

// test operator new

#include <new>
#include <cstddef>
#include <cassert>
#include <cstdint>
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

void test_throw_max_size() {
#ifndef TEST_HAS_NO_EXCEPTIONS
    std::set_new_handler(my_new_handler);
    try
    {
        void* vp = operator new (std::numeric_limits<std::size_t>::max(),
                                 static_cast<std::align_val_t>(32));
        ((void)vp);
        assert(false);
    }
    catch (std::bad_alloc&)
    {
        assert(new_handler_called == 1);
    }
    catch (...)
    {
        assert(false);
    }
#endif
}

int main(int, char**)
{
    {
        A* ap = new A;
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
        test_throw_max_size();
    }

  return 0;
}
