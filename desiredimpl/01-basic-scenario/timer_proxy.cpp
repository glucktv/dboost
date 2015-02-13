#include <timer_proxy.hpp>
#include <dbus/dbus.h>                    
#include <boost/scoped_ptr.h>
#include <ptrutils.h> // must be included to correctly free dbus structures

namespace dboost
{

#define DBOOST_CHECK_RESULT(call) 
#define DBOOST_CHECK_PTR(call) 
#define DBOOST_THROW_IF_NULL(value, error)

timer_proxy::timer_proxy()
{}

timer_proxy::~timer_proxy() {}

// here arguments are called a0 .. aN to avoid naming clashes, result is called r
long timer_proxy::add_timer(const long& a0)
{
  // create caller (name, arguments)
  boost::scoped_ptr<DBusMessage> msg(DBOOST_CHECK_PTR(dbus_message_new_method_call(m_name, m_objPath, m_ifcName, "add_timer")));
  DBOOST_CHECK_RESULT(dbus_message_append_args(a0, DBUS_TYPE_INT32, &timeout_ms, DBUS_TYPE_INVALID));

  DBusError err;

  // call synchronously
  boost::scoped_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_client.bus, msg, TIMEOUT_MS, &err));
  DBOOST_CHECK_RESULT_WITH_ERR(result, err);

  // unpack output parameters
  long r;                                                                    
  DBOOST_THROW_IF_NULL(dbus_message_get_args(reply, &err, DBUS_TYPE_INT32, &r, DBUS_TYPE_INVALID));        
  return r;
}

void timer_proxy::remove_timer(const long& a0)
{
  // create caller (name, arguments)
  boost::scoped_ptr<DBusMessage> msg(DBOOST_CHECK_PTR(dbus_message_new_method_call(m_name, m_objPath, m_ifcName, "remove_timer")));
  DBOOST_CHECK_RESULT(dbus_message_append_args(a0, DBUS_TYPE_INT32, &timeout_ms, DBUS_TYPE_INVALID));

  DBusError err;

  // call synchronously
  boost::scoped_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_client.bus, &msg, TIMEOUT_MS, &err));
  DBOOST_CHECK_RESULT_WITH_ERR(result, err);

  // unpack output parameters
  long r;                                                                    
  DBOOST_THROW_IF_NULL(dbus_message_get_args(reply, &err, DBUS_TYPE_INT32, &r, DBUS_TYPE_INVALID));        
  return r;
}


} // namespace dboost

