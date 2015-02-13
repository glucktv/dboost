#ifndef TIMER_ADAPTOR_H
#define TIMER_ADAPTOR_H

#include <timer.hpp>

namespace dboost
{

class timer_proxy: virtual public timer
{
public:
  timer_proxy(client& c, const std::string& path);
  virtual ~timer_proxy() {}
  virtual long add_timer(const long& timeout_ms); // override;
  virtual void remove_timer(const long& timer_id); // override;

private:
  client& m_client;
  const std::string m_path;
  const std::string m_ifc_name;
};

} // namespace dboost

#endif
