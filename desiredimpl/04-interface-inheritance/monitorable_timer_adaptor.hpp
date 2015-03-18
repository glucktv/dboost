#ifndef MONITORABLE_TIMER_ADAPTOR_HPP
#define MONITORABLE_TIMER_ADAPTOR_HPP
#include <map>
#include <dbus/dbus.h>
#include <dbus_ptr.h>
#include <timer_adaptor.hpp>
#include "monitorable_timer.hpp"


namespace dboost_test
{

class monitorable_timer_adaptor : public dboost_test::timer_adaptor
{
public:
  monitorable_timer_adaptor(dboost::server& s);
  virtual ~monitorable_timer_adaptor();

  virtual DBusHandlerResult handle_message(DBusConnection* connection, DBusMessage* message) override;

  virtual void add_object(dboost_test::timer* t, const std::string& name) override;
  virtual void remove_object(const std::string& name) override;

private:
  dboost::server& m_server;
  std::map<std::string, dboost_test::monitorable_timer*> m_objects;

  static const char* INTERFACE_NAME;

  dboost::dbus_ptr<DBusMessage> call_set_monitor(dboost_test::monitorable_timer* t, DBusMessage* m);
  dboost::dbus_ptr<DBusMessage> call_reset_monitor(dboost_test::monitorable_timer* t, DBusMessage* m);
};

}

#endif // MONITORABLE_TIMER_ADAPTOR_HPP
