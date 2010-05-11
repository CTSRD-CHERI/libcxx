//===----------------------------------------------------------------------===//
//
// ΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚΚThe LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <queue>

// bool empty() const;

#include <queue>
#include <cassert>

int main()
{
    std::queue<int> q;
    assert(q.empty());
    q.push(1);
    assert(!q.empty());
    q.pop();
    assert(q.empty());
}
