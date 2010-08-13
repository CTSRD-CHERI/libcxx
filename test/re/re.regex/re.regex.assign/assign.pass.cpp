//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <regex>

// template <class charT, class traits = regex_traits<charT>> class basic_regex;

// basic_regex& assign(const basic_regex& that);

#include <regex>
#include <cassert>

int main()
{
    std::regex r1("(a([bc]))");
    std::regex r2;
    r2.assign(r1);
    assert(r2.flags() == std::regex::ECMAScript);
    assert(r2.mark_count() == 2);
}
