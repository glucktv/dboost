#include "timer_impl.h"
#include <timer_observer_proxy.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <memory>

using namespace std;
using namespace boost::asio;

namespace dboost_test
{


bool timer_impl::observer_less::operator()(const dboost::ptr<timer_observer>& lhs,
                                           const dboost::ptr<timer_observer>& rhs)
{
    // only applicable for timer_observer_proxies
    auto plhs = dynamic_cast<const timer_observer_proxy*>(lhs.get());
    auto prhs = dynamic_cast<const timer_observer_proxy*>(rhs.get());
    assert(plhs && prhs);
    return  plhs->get_bus_name() < prhs->get_bus_name() ||
           (plhs->get_bus_name() == prhs->get_bus_name() &&
            plhs->get_obj_name() < prhs->get_obj_name());
}

void timer_handler(const boost::system::error_code& ec,
                   dboost::ptr<timer_observer> obs,
                   boost::asio::deadline_timer* tm,
                   long timeout_ms)
{
    clog << __FUNCTION__ << endl;
    if (ec) {
        clog << "error in timer_handler" << endl;
        return;
    }
    obs->on_timeout();
    tm->expires_from_now(boost::posix_time::millisec(timeout_ms));
    tm->async_wait(std::bind(timer_handler, std::placeholders::_1, obs, tm, timeout_ms));
    clog << __FUNCTION__ << " end" << endl;
}


long timer_impl::add_timer(const long timeout_ms, dboost::ptr<timer_observer> obs)
{  
    clog << __FUNCTION__ << " to = " << timeout_ms << " id = " << m_last_id << endl;

    unique_ptr<deadline_timer> tm(new deadline_timer(m_ios));
    tm->expires_from_now(boost::posix_time::millisec(timeout_ms));
    tm->async_wait(std::bind(timer_handler, std::placeholders::_1, obs, tm.get(), timeout_ms));
    m_timers[obs] = std::move(tm);
    clog << __FUNCTION__ << " end" << endl;
    return m_last_id++;
}

void timer_impl::remove_timer(dboost::ptr<timer_observer> obs)
{
    clog << __FUNCTION__ << endl;
    auto iter = m_timers.find(obs);
    if (iter == m_timers.end()) {
        clog << __FUNCTION__ << ": could not find a timer" << endl;
        return;
    }
    m_timers.erase(iter);
}

}
