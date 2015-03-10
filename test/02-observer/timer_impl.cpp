#include "timer_impl.h"
#include <iostream>
#include <thread>

using namespace std;

namespace dboost_test
{

void th_fun(dboost::ref<timer_observer> obs)
{
    clog << __FUNCTION__ << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    clog << __FUNCTION__ << endl;
    obs.get().on_timeout();
}

long timer_impl::add_timer(const long timeout_ms, dboost::ref<timer_observer> obs)
{  
    clog << __FUNCTION__ << " to = " << timeout_ms << " id = " << m_last_id << endl;
    m_timers[m_last_id] = timeout_ms;
    std::thread (th_fun, obs).detach();
    //th.detach();
    return m_last_id++;
}

void timer_impl::remove_timer(dboost::ref<timer_observer> obs)
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
