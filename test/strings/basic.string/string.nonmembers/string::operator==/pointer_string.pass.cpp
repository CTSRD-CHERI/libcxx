//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <string>

// template<class charT, class traits, class Allocator> 
//   bool operator==(const charT* lhs, const basic_string<charT,traits,Allocator>& rhs);

#include <string>
#include <cassert>

template <class S>
void
test(const typename S::value_type* lhs, const S& rhs, bool x)
{
    assert((lhs == rhs) == x);
}

typedef std::string S;

int main()
{
    test("", S(""), true);
    test("", S("abcde"), false);
    test("", S("abcdefghij"), false);
    test("", S("abcdefghijklmnopqrst"), false);
    test("abcde", S(""), false);
    test("abcde", S("abcde"), true);
    test("abcde", S("abcdefghij"), false);
    test("abcde", S("abcdefghijklmnopqrst"), false);
    test("abcdefghij", S(""), false);
    test("abcdefghij", S("abcde"), false);
    test("abcdefghij", S("abcdefghij"), true);
    test("abcdefghij", S("abcdefghijklmnopqrst"), false);
    test("abcdefghijklmnopqrst", S(""), false);
    test("abcdefghijklmnopqrst", S("abcde"), false);
    test("abcdefghijklmnopqrst", S("abcdefghij"), false);
    test("abcdefghijklmnopqrst", S("abcdefghijklmnopqrst"), true);
}
