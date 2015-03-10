/*
 * server.cpp
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#include "server.h"
#include "dbus_ptr.h"
#include "exception.h"
#include "adaptor.h"
#include <cassert>
#include <iostream>
#include <dbus/dbus.h>
#include <utils.h>

namespace dboost
{

using namespace std;

DBusObjectPathVTable server::s_vtbl = {
    nullptr,
    server::message_func,
    nullptr
};

server::server(const std::string& name)
    : m_name(name),
      m_connection(create_connection())
{
    error err;
    if (dbus_bus_request_name(m_connection.get(), m_name.c_str(), 0, &err) == -1) {
        throw communication_error(err);
    }
}

server::~server()
{
    error err;
    dbus_bus_release_name(m_connection.get(), m_name.c_str(), &err);
}

dbus_ptr<DBusConnection> server::get_connection()
{
    return m_connection;
}

std::string server::get_unique_name()
{
    return dbus_bus_get_unique_name(m_connection.get());
}

DBusHandlerResult
server::message_func(DBusConnection* connection, DBusMessage* message,
    				 void* user_data)
{
    auto s = reinterpret_cast<server*>(user_data);
    assert(s != nullptr);
    return s->message_func_impl(connection, message);
}

DBusHandlerResult
server::message_func_impl(DBusConnection* connection, DBusMessage* message)
{
    clog << __FUNCTION__ << endl;
    auto iter = m_adaptors.find(dbus_message_get_interface(message));
    if (iter == m_adaptors.end()) {
    	clog << "Warning: unknown interface " << dbus_message_get_interface(message) << endl;
    	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    return iter->second->handle_message(connection, message);
}

void server::run()
{
    while (dbus_connection_read_write_dispatch (m_connection.get(), -1));
}

#if 0
void server::register_adaptor(adaptor* a, const std::string& ifc_name)
{
    clog << __FUNCTION__ << " ifc = " << ifc_name << endl;
    m_adaptors[ifc_name] = a;
}

void server::unregister_adaptor(adaptor* /*a*/, const std::string& ifc_name)
{
    clog << __FUNCTION__ << " ifc = " << ifc_name << endl;
    auto iter = m_adaptors.find(ifc_name);
    if (iter == m_adaptors.end()) {
        cerr << "Trying to unregister inexistent interface " << ifc_name << endl;
    }
    else {
        m_adaptors.erase(iter);
    }
}

void server::register_object(const std::string& name)
{
    clog << __FUNCTION__ << " name = " << name << endl;
    DBOOST_CHECK(dbus_connection_register_object_path(m_connection.get(), name.c_str(), &s_vtbl, this));
}

void server::unregister_object(const std::string& name)
{
    clog << __FUNCTION__ << " name = " << name << endl;
    DBOOST_CHECK(dbus_connection_unregister_object_path(m_connection.get(), name.c_str()));
}
#endif


} // namespace dboost

