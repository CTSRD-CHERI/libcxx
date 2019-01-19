//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <locale>

// template <class CharT>
// class ctype
//     : public locale::facet,
//       public ctype_base
// {
// public:
//     typedef CharT char_type;
// };

#include <locale>
#include <type_traits>
#include <cassert>

int main()
{
    std::locale l = std::locale::classic();
    {
        assert(std::has_facet<std::ctype<wchar_t> >(l));
        const std::ctype<wchar_t>& f = std::use_facet<std::ctype<wchar_t> >(l);
        ((void)f); // Prevent unused warning
        {
            (void)std::ctype<wchar_t>::id;
        }
        static_assert((std::is_same<std::ctype<wchar_t>::char_type, wchar_t>::value), "");
        static_assert((std::is_base_of<std::ctype_base, std::ctype<wchar_t> >::value), "");
        static_assert((std::is_base_of<std::locale::facet, std::ctype<wchar_t> >::value), "");
    }
}
