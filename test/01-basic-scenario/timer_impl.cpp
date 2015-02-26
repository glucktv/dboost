#include "timer_impl.h"
#include <iostream>

using namespace std;

namespace dboost_test
{

long timer_impl::add_timer(const long& timeout_ms)
{  
    clog << __FUNCTION__ << " to = " << timeout_ms << " id = " << m_last_id << endl;
    m_timers[m_last_id] = timeout_ms;
    return m_last_id++;
}

void timer_impl::remove_timer(const long& timer_id)
{
    clog << __FUNCTION__ << " id = " << timer_id << endl;
    auto it = m_timers.find(timer_id);
    if (it == m_timers.end()) {
        cerr << "Could not find id = " << timer_id << endl;
    }
    else {
        m_timers.erase(it);
    }
}

}
