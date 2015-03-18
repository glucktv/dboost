#ifndef TIMER_PROXY_HPP
#define TIMER_PROXY_HPP
#include <string>
#include <dbus_ptr.h>
#include "timer.hpp"


namespace dboost_test
{

class timer_proxy: virtual public dboost_test::timer
{
private:
  dboost::dbus_ptr<DBusConnection> m_connection;
  const std::string m_bus_name;
  const std::string m_obj_name;
  static const char* s_ifc_name;
public:
  timer_proxy(dboost::dbus_ptr<DBusConnection> conn, const std::string& bus_name, const std::string& obj_name);

  virtual long add_timer(const long timeout_ms) throw (dboost_test::wrong_timeout_exception) override;
  virtual void remove_timer(const long id) override;
};

}

#endif // TIMER_PROXY_HPP
