#ifndef TIMER_PROXY_H
#define TIMER_PROXY_H

#include <timer.hpp>
#include <dbus_ptr.h>

namespace dboost
{

class timer_proxy: virtual public dboost_test::timer
{
public:
  timer_proxy(dbus_ptr<DBusConnection> conn, const std::string& bus_name, const std::string& obj_name);

  virtual long add_timer(const long& timeout_ms) override;
  virtual void remove_timer(const long& timer_id) override;

private:
  dbus_ptr<DBusConnection> m_connection;
  const std::string m_bus_name;
  const std::string m_obj_name;
  static const char* s_ifc_name;
};

} // namespace dboost

#endif
