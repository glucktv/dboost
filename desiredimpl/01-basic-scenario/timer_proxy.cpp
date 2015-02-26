#include <timer_proxy.h>
#include <dbus/dbus.h>                    
#include <exception.h>
#include <iostream>

namespace dboost
{

const char* timer_proxy::s_ifc_name = "org.dboost.timer";
const int TIMEOUT_MS = 5000;

timer_proxy::timer_proxy(dbus_ptr<DBusConnection> conn,
				const std::string& bus_name, const std::string& obj_name)
    : m_connection(conn),
      m_bus_name(bus_name),
      m_obj_name(obj_name)
{
}

// here arguments are called a0 .. aN to avoid naming clashes, result is called r
long timer_proxy::add_timer(const long& a0)
{
	// create caller (name, arguments)
	dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "add_timer")));
	DBusMessageIter it;
	dbus_message_iter_init_append(msg.get(), &it);
	DBOOST_CHECK(dbus_message_iter_append_basic(&it, DBUS_TYPE_INT64, &a0));

	error err;

	// call synchronously
	dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));
	DBOOST_CHECK_WITH_ERR(reply, err);
	if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
		throw exception(dbus_message_get_error_name(reply.get()));
	}

	// unpack output parameters
	long r;
	dbus_message_iter_init(reply.get(), &it);
	if (dbus_message_iter_get_arg_type(&it) != DBUS_TYPE_INT64) {
		throw exception("Incorrect response");
	}
	dbus_message_iter_get_basic(&it, &r);
	return r;
}


void timer_proxy::remove_timer(const long& a0)
{
	// create caller (name, arguments)
	dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "remove_timer")));
	DBusMessageIter it;
	dbus_message_iter_init_append(msg.get(), &it);
	DBOOST_CHECK(dbus_message_iter_append_basic(&it, DBUS_TYPE_INT64, &a0));

	error err;

	// call synchronously
	dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

	DBOOST_CHECK_WITH_ERR(reply, err);
	if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
	  throw exception(dbus_message_get_error_name(reply.get()));
	}

}


} // namespace dboost

