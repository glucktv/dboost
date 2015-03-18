#ifndef MONITORABLE_TIMER_HPP
#define MONITORABLE_TIMER_HPP


#include "timer.hpp"
namespace dboost_test
{

class monitorable_timer : virtual public dboost_test::timer 
{
public:
  virtual ~monitorable_timer() {};
  virtual void set_monitor() = 0;
  virtual void reset_monitor() = 0;
};

};

#endif // MONITORABLE_TIMER_HPP
