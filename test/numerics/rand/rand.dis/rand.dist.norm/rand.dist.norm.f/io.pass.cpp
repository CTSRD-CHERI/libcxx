//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <random>

// template<class RealType = double>
// class fisher_f_distribution

// template <class CharT, class Traits, class RealType>
// basic_ostream<CharT, Traits>&
// operator<<(basic_ostream<CharT, Traits>& os,
//            const fisher_f_distribution<RealType>& x);

// template <class CharT, class Traits, class RealType>
// basic_istream<CharT, Traits>&
// operator>>(basic_istream<CharT, Traits>& is,
//            fisher_f_distribution<RealType>& x);

#include <random>
#include <sstream>
#include <cassert>

int main()
{
    {
        typedef std::fisher_f_distribution<> D;
        D d1(7, 5);
        std::ostringstream os;
        os << d1;
        std::istringstream is(os.str());
        D d2;
        is >> d2;
        assert(d1 == d2);
    }
}
