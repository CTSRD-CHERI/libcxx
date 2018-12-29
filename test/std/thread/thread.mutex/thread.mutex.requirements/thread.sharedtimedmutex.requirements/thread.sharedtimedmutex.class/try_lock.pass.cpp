//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: libcpp-has-no-threads
// UNSUPPORTED: c++98, c++03, c++11

// <shared_mutex>

// class shared_timed_mutex;

// bool try_lock();

#include <shared_mutex>
#include <thread>
#include <cstdlib>
#include <cassert>

std::shared_timed_mutex m;

typedef std::chrono::system_clock Clock;
typedef Clock::time_point time_point;
typedef Clock::duration duration;
typedef std::chrono::milliseconds ms;
typedef std::chrono::nanoseconds ns;

#if !defined(TEST_SLOW_HOST)
ms WaitTime = ms(250);
#else
ms WaitTime = ms(750);
#endif

#if !defined(TEST_HAS_SANITIZERS) && !defined(TEST_SLOW_HOST)
ms Tolerance = ms(200);
#else
ms Tolerance = ms(200 * 5);
#endif


void f()
{
    time_point t0 = Clock::now();
    assert(!m.try_lock());
    assert(!m.try_lock());
    assert(!m.try_lock());
    while(!m.try_lock())
        ;
    time_point t1 = Clock::now();
    m.unlock();
    ns d = t1 - t0 - WaitTime;
    assert(d < Tolerance);  // within 200ms
}

int main()
{
    m.lock();
    std::thread t(f);
    std::this_thread::sleep_for(WaitTime);
    m.unlock();
    t.join();
}
