//===----------------------------------------------------------------------===//
//
// ΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚThe LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <stack>

// template <class Alloc>
//   stack(const container_type& c, const Alloc& a);

#include <stack>
#include <cassert>

#include "../../../../test_allocator.h"
#include "../../../../MoveOnly.h"

#ifdef _LIBCPP_MOVE

template <class C>
C
make(int n)
{
    C c;
    for (int i = 0; i < n; ++i)
        c.push_back(MoveOnly(i));
    return c;
}

typedef std::deque<MoveOnly, test_allocator<MoveOnly> > C;

template <class T>
struct test
    : public std::stack<T, C>
{
    typedef std::stack<T, C> base;
    typedef test_allocator<MoveOnly>      allocator_type;
    typedef typename base::container_type container_type;

    explicit test(const allocator_type& a) : base(a) {}
    test(const container_type& c, const allocator_type& a) : base(c, a) {}
    test(container_type&& c, const allocator_type& a) : base(std::move(c), a) {}
    test(test&& q, const allocator_type& a) : base(std::move(q), a) {}
    allocator_type get_allocator() {return this->c.get_allocator();}
};

#endif

int main()
{
#ifdef _LIBCPP_MOVE
    test<MoveOnly> q(make<C>(5), test_allocator<MoveOnly>(4));
    assert(q.get_allocator() == test_allocator<MoveOnly>(4));
    assert(q.size() == 5);
#endif
}
