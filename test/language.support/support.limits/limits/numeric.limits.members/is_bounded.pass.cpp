//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// test numeric_limits

// is_bounded

#include <limits>

template <class T, bool expected>
void
test()
{
    static_assert(std::numeric_limits<T>::is_bounded == expected, "is_bounded test 1");
    static_assert(std::numeric_limits<const T>::is_bounded == expected, "is_bounded test 2");
    static_assert(std::numeric_limits<volatile T>::is_bounded == expected, "is_bounded test 3");
    static_assert(std::numeric_limits<const volatile T>::is_bounded == expected, "is_bounded test 4");
}

int main()
{
    test<bool, true>();
    test<char, true>();
    test<signed char, true>();
    test<unsigned char, true>();
    test<wchar_t, true>();
#ifndef _LIBCPP_HAS_NO_UNICODE_CHARS
    test<char16_t, true>();
    test<char32_t, true>();
#endif
    test<short, true>();
    test<unsigned short, true>();
    test<int, true>();
    test<unsigned int, true>();
    test<long, true>();
    test<unsigned long, true>();
    test<long long, true>();
    test<unsigned long long, true>();
    test<float, true>();
    test<double, true>();
    test<long double, true>();
}
