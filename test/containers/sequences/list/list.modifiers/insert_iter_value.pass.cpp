//===----------------------------------------------------------------------===//
//
// ΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚThe LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <list>

// iterator insert(const_iterator position, const value_type& x);

#include <list>
#include <cstdlib>
#include <cassert>

int throw_next = 0xFFFF;
int count = 0;

void* operator new(std::size_t s) throw(std::bad_alloc)
{
    if (throw_next == 0)
        throw std::bad_alloc();
    --throw_next;
    ++count;
    return std::malloc(s);
}

void  operator delete(void* p) throw()
{
    --count;
    std::free(p);
}

int main()
{
    int a1[] = {1, 2, 3};
    int a2[] = {1, 4, 2, 3};
    std::list<int> l1(a1, a1+3);
    std::list<int>::iterator i = l1.insert(next(l1.cbegin()), 4);
    assert(i == next(l1.begin()));
    assert(l1.size() == 4);
    assert(distance(l1.begin(), l1.end()) == 4);
    assert(l1 == std::list<int>(a2, a2+4));
    throw_next = 0;
    int save_count = count;
    try
    {
        i = l1.insert(i, 5);
        assert(false);
    }
    catch (...)
    {
    }
    throw_next = 0xFFFF;
    assert(save_count == count);
    assert(l1 == std::list<int>(a2, a2+4));
}
