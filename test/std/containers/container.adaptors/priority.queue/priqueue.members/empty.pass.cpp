//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <queue>

// priority_queue();

// bool empty() const;

#include <queue>
#include <cassert>

int main()
{
    std::priority_queue<int> q;
    assert(q.empty());
    q.push(1);
    assert(!q.empty());
    q.pop();
    assert(q.empty());
}
