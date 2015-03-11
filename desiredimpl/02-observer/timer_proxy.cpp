#include <timer_proxy.h>
#include <timer_observer_proxy.h>
#include <timer_observer_adaptor.h>
#include <dbus/dbus.h>                    
#include <exception.h>
#include <iostream>
#include <serializer.h>

namespace dboost_test
{

const char* timer_proxy::s_ifc_name = "org.dboost.timer";
const int TIMEOUT_MS = 5000;

timer_proxy::timer_proxy(dboost::server& server,
    			const std::string& bus_name, const std::string& obj_name)
    : m_server(server),
      m_connection(m_server.get_connection()),
      m_bus_name(bus_name),
      m_obj_name(obj_name)
{
}

// here arguments are called a0 .. aN to avoid naming clashes, result is called r
long timer_proxy::add_timer(const long a0, dboost::ptr<timer_observer> a1)
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "add_timer")));
    dboost::oserializer os(msg.get());
    os & a0;

    timer_observer_proxy* p1 = dynamic_cast<timer_observer_proxy*>(a1.get());
    if (p1 != nullptr) {
        os & p1->get_bus_name() & p1->get_obj_name();
    }
    else {
        os & m_server.get_unique_name() & m_server.get_object_name(a1.get());
    }

    // call synchronously
    dboost::error err;
    dboost::dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

    // check if there was an error
    DBOOST_CHECK_WITH_ERR(reply, err);
    if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
    	throw dboost::exception(dbus_message_get_error_name(reply.get()));
    }

    // unpack output parameters
    dboost::iserializer is(reply.get());
    long r;
    is & r;
    return r;
}

void timer_proxy::remove_timer(dboost::ptr<timer_observer> a0)
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "remove_timer")));
    dboost::oserializer os(msg.get());
    //os & a0;
    timer_observer_proxy* p0 = dynamic_cast<timer_observer_proxy*>(a0.get());
    if (p0 != nullptr) {
        os & p0->get_bus_name() & p0->get_obj_name();
    }
    else {
        os & m_server.get_unique_name() & m_server.get_object_name(a0.get());
    }

    // call synchronously
    dboost::error err;
    dboost::dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

    // check if there was an error
    DBOOST_CHECK_WITH_ERR(reply, err);
    if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
      throw dboost::exception(dbus_message_get_error_name(reply.get()));
    }
}

} // namespace dboost

