#include "timer_impl.h"
#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;

namespace dboost_test
{

void th_fun(dboost::ptr<timer_observer> obs)
{
    clog << __FUNCTION__ << endl;
    while (true) {
        usleep(2000 * 1000);
        clog << __FUNCTION__ << endl;
        obs->on_timeout();
    }
}

long timer_impl::add_timer(const long timeout_ms, dboost::ptr<timer_observer> obs)
{  
    clog << __FUNCTION__ << " to = " << timeout_ms << " id = " << m_last_id << endl;
    m_timers[m_last_id] = obs;
    std::thread (th_fun, obs).detach();
    //th.detach();
    return m_last_id++;
}

void timer_impl::remove_timer(dboost::ptr<timer_observer> obs)
{
    clog << __FUNCTION__ << endl;
    /*
    auto it = m_timers.find(timer_id);
    if (it == m_timers.end()) {
        cerr << "Could not find id = " << timer_id << endl;
    }
    else {
        m_timers.erase(it);
    }
    */
}

}
