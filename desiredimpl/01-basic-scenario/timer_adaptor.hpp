#ifndef TIMER_ADAPTOR_H
#define TIMER_ADAPTOR_H

#include <timer.hpp>

namespace dboost
{

class timer_adaptor: public timer
{
public:
  virtual ~timer_adaptor() {}
  virtual long add_timer(const long& timeout_ms); // override;
  virtual void remove_timer(const long& timer_id); // override;
};

} // namespace dboost

#endif
