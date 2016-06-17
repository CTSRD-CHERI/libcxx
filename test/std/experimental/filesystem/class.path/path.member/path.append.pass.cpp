//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

// <experimental/filesystem>

// class path

// path& operator/=(path const&)
// template <class Source>
//      path& operator/=(Source const&);
// template <class Source>
//      path& append(Source const&);
// template <class InputIterator>
//      path& append(InputIterator first, InputIterator last);


#include <experimental/filesystem>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "test_iterators.h"
#include "count_new.hpp"
#include "filesystem_test_helper.hpp"

namespace fs = std::experimental::filesystem;

struct AppendOperatorTestcase {
  MultiStringType lhs;
  MultiStringType rhs;
  MultiStringType expect;
};

#define S(Str) MKSTR(Str)
const AppendOperatorTestcase Cases[] =
    {
        {S(""),     S(""),      S("")}
      , {S("p1"),   S("p2"),    S("p1/p2")}
      , {S("p1/"),  S("p2"),    S("p1/p2")}
      , {S("p1"),   S("/p2"),   S("p1/p2")}
      , {S("p1/"),  S("/p2"),   S("p1//p2")}
      , {S("p1"),   S("\\p2"),  S("p1/\\p2")}
      , {S("p1\\"), S("p2"),  S("p1\\/p2")}
      , {S("p1\\"), S("\\p2"),  S("p1\\/\\p2")}
      , {S("p1"),   S(""),      S("p1")}
      , {S(""),     S("p2"),    S("p2")}
    };


const AppendOperatorTestcase LongLHSCases[] =
    {
        {S("p1"),   S("p2"),    S("p1/p2")}
      , {S("p1/"),  S("p2"),    S("p1/p2")}
      , {S("p1"),   S("/p2"),   S("p1/p2")}
    };
#undef S


// The append operator may need to allocate a temporary buffer before a code_cvt
// conversion. Test if this allocation occurs by:
//   1. Create a path, `LHS`, and reserve enough space to append `RHS`.
//      This prevents `LHS` from allocating during the actual appending.
//   2. Create a `Source` object `RHS`, which represents a "large" string.
//      (The string must not trigger the SSO)
//   3. Append `RHS` to `LHS` and check for the expected allocation behavior.
template <class CharT>
void doAppendSourceAllocTest(AppendOperatorTestcase const& TC)
{
  using namespace fs;
  using Ptr = CharT const*;
  using Str = std::basic_string<CharT>;
  using InputIter = input_iterator<Ptr>;

  const Ptr L = TC.lhs;
  Str RShort = (Ptr)TC.rhs;
  Str EShort = (Ptr)TC.expect;
  assert(RShort.size() >= 2);
  CharT c = RShort.back();
  RShort.append(100, c);
  EShort.append(100, c);
  const Ptr R = RShort.data();
  const Str& E = EShort;
  std::size_t ReserveSize = E.size() + 3;
  // basic_string
  {
    path LHS(L); PathReserve(LHS, ReserveSize);
    Str  RHS(R);
    {
      DisableAllocationGuard g;
      LHS /= RHS;
    }
    assert(LHS == E);
  }
  // CharT*
  {
    path LHS(L); PathReserve(LHS, ReserveSize);
    Ptr RHS(R);
    {
      DisableAllocationGuard g;
      LHS /= RHS;
    }
    assert(LHS == E);
  }
  {
    path LHS(L); PathReserve(LHS, ReserveSize);
    Ptr RHS(R);
    {
      DisableAllocationGuard g;
      LHS.append(RHS, StrEnd(RHS));
    }
    assert(LHS == E);
  }
  // input iterator - For non-native char types, appends needs to copy the
  // iterator range into a contigious block of memory before it can perform the
  // code_cvt conversions.
  // For "char" no allocations will be performed because no conversion is
  // required.
  bool DisableAllocations = std::is_same<CharT, char>::value;
  {
    path LHS(L); PathReserve(LHS, ReserveSize);
    InputIter RHS(R);
    {
      RequireAllocationGuard  g; // requires 1 or more allocations occur by default
      if (DisableAllocations) g.requireExactly(0);
      LHS /= RHS;
    }
    assert(LHS == E);
  }
  {
    path LHS(L); PathReserve(LHS, ReserveSize);
    InputIter RHS(R);
    InputIter REnd(StrEnd(R));
    {
      RequireAllocationGuard g;
      if (DisableAllocations) g.requireExactly(0);
      LHS.append(RHS, REnd);
    }
    assert(LHS == E);
  }
}

template <class CharT>
void doAppendSourceTest(AppendOperatorTestcase const& TC)
{
  using namespace fs;
  using Ptr = CharT const*;
  using Str = std::basic_string<CharT>;
  using InputIter = input_iterator<Ptr>;
  const Ptr L = TC.lhs;
  const Ptr R = TC.rhs;
  const Ptr E = TC.expect;
  // basic_string
  {
    path LHS(L);
    Str RHS(R);
    path& Ref = (LHS /= RHS);
    assert(LHS == E);
    assert(&Ref == &LHS);
  }
  {
    path LHS(L);
    Str RHS(R);
    path& Ref = LHS.append(RHS);
    assert(LHS == E);
    assert(&Ref == &LHS);
  }
  // Char*
  {
    path LHS(L);
    Str RHS(R);
    path& Ref = (LHS /= RHS);
    assert(LHS == E);
    assert(&Ref == &LHS);
  }
  {
    path LHS(L);
    Ptr RHS(R);
    path& Ref = LHS.append(RHS);
    assert(LHS == E);
    assert(&Ref == &LHS);
  }
  {
    path LHS(L);
    Ptr RHS(R);
    path& Ref = LHS.append(RHS, StrEnd(RHS));
    assert(LHS == E);
    assert(&Ref == &LHS);
  }
  // iterators
  {
    path LHS(L);
    InputIter RHS(R);
    path& Ref = (LHS /= RHS);
    assert(LHS == E);
    assert(&Ref == &LHS);
  }
  {
    path LHS(L); InputIter RHS(R);
    path& Ref = LHS.append(RHS);
    assert(LHS == E);
    assert(&Ref == &LHS);
  }
  {
    path LHS(L);
    InputIter RHS(R);
    InputIter REnd(StrEnd(R));
    path& Ref = LHS.append(RHS, REnd);
    assert(LHS == E);
    assert(&Ref == &LHS);
  }
}

int main()
{
  using namespace fs;
  for (auto const & TC : Cases) {
    {
      path LHS((const char*)TC.lhs);
      path RHS((const char*)TC.rhs);
      path& Ref = (LHS /= RHS);
      assert(LHS == (const char*)TC.expect);
      assert(&Ref == &LHS);
    }
    doAppendSourceTest<char>    (TC);
    doAppendSourceTest<wchar_t> (TC);
    doAppendSourceTest<char16_t>(TC);
    doAppendSourceTest<char32_t>(TC);
  }
  for (auto const & TC : LongLHSCases) {
    doAppendSourceAllocTest<char>(TC);
    doAppendSourceAllocTest<wchar_t>(TC);
  }
}
