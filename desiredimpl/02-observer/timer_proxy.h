#ifndef TIMER_PROXY_H
#define TIMER_PROXY_H

#include <timer.hpp>
#include <dbus_ptr.h>
#include <ptr.h>
#include <server.h>

namespace dboost_test
{

class timer_proxy: virtual public dboost_test::timer
{
public:
  timer_proxy(dboost::server& s, const std::string& bus_name, const std::string& obj_name);

  virtual long add_timer(const long timeout_ms, dboost::ptr<timer_observer> obs) override;
  virtual void remove_timer(dboost::ptr<timer_observer> obs) override;

private:
  dboost::server& m_server;
  dboost::dbus_ptr<DBusConnection> m_connection;
  const std::string m_bus_name;
  const std::string m_obj_name;
  static const char* s_ifc_name;
};

} // namespace dboost_test

#endif
