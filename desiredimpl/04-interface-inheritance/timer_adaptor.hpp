#ifndef TIMER_ADAPTOR_HPP
#define TIMER_ADAPTOR_HPP
#include <map>
#include <dbus/dbus.h>
#include <dbus_ptr.h>
#include <adaptor.h>
#include "timer.hpp"


namespace dboost_test
{

class timer_adaptor : public virtual dboost::adaptor
{
public:
  timer_adaptor(dboost::server& s);
  virtual ~timer_adaptor();

  // adaptor
  virtual DBusHandlerResult handle_message(DBusConnection* connection, DBusMessage* message) override;

  virtual void add_object(dboost_test::timer* t, const std::string& name);
  virtual void remove_object(const std::string& name);

private:
  dboost::server& m_server;
  std::map<std::string, dboost_test::timer*> m_objects;

  static const char* INTERFACE_NAME;

  dboost::dbus_ptr<DBusMessage> call_add_timer(dboost_test::timer* t, DBusMessage* m);
  dboost::dbus_ptr<DBusMessage> call_remove_timer(dboost_test::timer* t, DBusMessage* m);
};

}

#endif // TIMER_ADAPTOR_HPP
