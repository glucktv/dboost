
#ifndef TIMER_IMPL_H
#define TIMER_IMPL_H

#define BOOST_ASIO_HAS_MOVE

#include "timer.hpp"
#include <map>
#include <boost/asio.hpp>

namespace dboost_test
{

class timer_impl: public timer
{
public:
    timer_impl(boost::asio::io_service& ios): m_last_id(0), m_ios(ios) {}
    virtual long add_timer(const long timeout_ms, dboost::ptr<timer_observer> obs) override;
    virtual void remove_timer(dboost::ptr<timer_observer> obs) override;

private:
    long m_last_id;
    boost::asio::io_service& m_ios;

    struct observer_less
    {
        bool operator()(const dboost::ptr<timer_observer>& lhs, const dboost::ptr<timer_observer>& rhs);
    };

    //static bool less(dboost::ptr<timer_observer> lhs, dboost::ptr<timer_observer> rhs);
    std::map<dboost::ptr<timer_observer>,
             std::unique_ptr<boost::asio::deadline_timer>, observer_less> m_timers;
};

};

#endif // TIMER_IMPL_H
