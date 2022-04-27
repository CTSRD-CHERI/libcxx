//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// XFAIL: LIBCXX-AIX-FIXME

// <string>

// basic_string<charT,traits,Allocator>& operator=(charT c); // constexpr since C++20

#include <string>
#include <cassert>

#include "test_macros.h"
#include "min_allocator.h"

template <class S>
TEST_CONSTEXPR_CXX20 void
test(S s1, typename S::value_type s2)
{
    typedef typename S::traits_type T;
    s1 = s2;
    LIBCPP_ASSERT(s1.__invariants());
    assert(s1.size() == 1);
    assert(T::eq(s1[0], s2));
    assert(s1.capacity() >= s1.size());
}

TEST_CONSTEXPR_CXX20 bool test() {
  {
    typedef std::string S;
    test(S(), 'a');
    test(S("1"), 'a');
    test(S("123456789"), 'a');
    test(S("1234567890123456789012345678901234567890123456789012345678901234567890"), 'a');
  }
#if TEST_STD_VER >= 11
  {
    typedef std::basic_string<char, std::char_traits<char>, min_allocator<char>> S;
    test(S(), 'a');
    test(S("1"), 'a');
    test(S("123456789"), 'a');
    test(S("1234567890123456789012345678901234567890123456789012345678901234567890"), 'a');
  }
#endif

  return true;
}

int main(int, char**)
{
  test();
#if TEST_STD_VER > 17
  static_assert(test());
#endif

  return 0;
}
