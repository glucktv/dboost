
#ifndef TIMER_IMPL_H
#define TIMER_IMPL_H

#include "monitorable_timer.hpp"
#include <map>

namespace dboost_test
{

class monitorable_timer_impl: public monitorable_timer
{
public:
    monitorable_timer_impl(): m_last_id(0) {}
    virtual long add_timer(const long timeout_ms) throw (dboost_test::wrong_timeout_exception) override;
    virtual void remove_timer(const long timer_id) override;
    virtual void set_monitor() override;
    virtual void reset_monitor() override;

private:
    long m_last_id;
    std::map<long/*id*/, long/*timeout*/> m_timers;    
};

};

#endif // TIMER_IMPL_H
