
#ifndef TIMER_IMPL_H
#define TIMER_IMPL_H

#include "timer.hpp"
#include <map>

namespace dboost_test
{

class timer_impl: public timer
{
public:
    timer_impl(): m_last_id(0) {}
    virtual long add_timer(const long timeout_ms, dboost::ptr<timer_observer> obs) override;
    virtual void remove_timer(dboost::ptr<timer_observer> obs) override;

private:
    long m_last_id;
    std::map<long/*id*/, dboost::ptr<timer_observer>> m_timers;
};

};

#endif // TIMER_IMPL_H
