/*
 * timer_observer_proxy.cpp
 *
 *  Created on: 04.03.2015
 *      Author: stas
 */

#include "timer_observer_proxy.h"
#include <dbus/dbus.h>
#include <exception.h>
#include <iostream>
#include <serializer.h>

namespace dboost_test
{

const char* timer_observer_proxy::s_ifc_name = "org.dboost.timerobserver";
const int TIMEOUT_MS = 5000;

timer_observer_proxy::timer_observer_proxy(dboost::dbus_ptr<DBusConnection> conn, const std::string& bus_name, const std::string& obj_name)
    : m_connection(conn),
      m_bus_name(bus_name),
      m_obj_name(obj_name)

{
}

void dboost_test::timer_observer_proxy::on_timeout()
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "on_timeout")));

    // call asynchronously
    dbus_connection_send(m_connection.get(), msg.get(), nullptr);
}

} // namespace dboost_test

