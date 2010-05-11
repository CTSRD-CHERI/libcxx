//===----------------------------------------------------------------------===//
//
// ΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚThe LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <memory>

// template <class X>
// class auto_ptr
// { 
// public: 
//   typedef X element_type;
//   ...
// };

#include <memory>
#include <type_traits>

template <class T>
void
test()
{
    static_assert((std::is_same<typename std::auto_ptr<T>::element_type, T>::value), "");
}

int main()
{
    test<int>();
    test<double>();
    test<void>();
    std::auto_ptr<void> p;
}
