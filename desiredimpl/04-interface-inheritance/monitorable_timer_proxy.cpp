#include <iostream>
#include <dbus/dbus.h>
#include <exception.h>
#include <serializer.h>
#include "monitorable_timer_proxy.hpp"

namespace dboost_test
{

const char* monitorable_timer_proxy::s_ifc_name = "org.dboost.monitorable_timer";
const int TIMEOUT_MS = 5000;

monitorable_timer_proxy::monitorable_timer_proxy(dboost::dbus_ptr<DBusConnection> conn,
                        const std::string& bus_name, const std::string& obj_name)
    : timer_proxy(conn, bus_name, obj_name),
      m_connection(conn),
      m_bus_name(bus_name),
      m_obj_name(obj_name)
{
}

void monitorable_timer_proxy::set_monitor()
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "set_monitor")));
    ;

    // call synchronously
    dboost::error err;
    dboost::dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

    // check if there was an error
    DBOOST_CHECK_WITH_ERR(reply, err);
    if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
        throw dboost::exception(dbus_message_get_error_name(reply.get()));
    }

    // unpack output parameters
    ;
    return ;
}


void monitorable_timer_proxy::reset_monitor()
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "reset_monitor")));
    ;

    // call synchronously
    dboost::error err;
    dboost::dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

    // check if there was an error
    DBOOST_CHECK_WITH_ERR(reply, err);
    if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
        throw dboost::exception(dbus_message_get_error_name(reply.get()));
    }

    // unpack output parameters
    ;
    return ;
}


}
