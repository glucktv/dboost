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

namespace dboost
{

using namespace std;



namespace {

dbus_ptr<DBusConnection> create_connection()
{
    error err;
    dbus_ptr<DBusConnection> s(dbus_bus_get(DBUS_BUS_SESSION, &err));
    DBOOST_CHECK_WITH_ERR(s, err);
    return s;
}

}

DBusObjectPathVTable server::s_vtbl = {
    nullptr,
    server::message_func,
    nullptr
};

server::server()
    : m_connection(create_connection())
{
}

void server::register_adaptor(adaptor* a,
                              const std::string& name,
    					      const std::vector<std::string>& interfaces)
{
    for (auto s: interfaces) {
    	m_adaptors.insert(make_pair(s, a));
    }
    DBOOST_CHECK(dbus_connection_register_object_path(m_connection.get(), name.c_str(), &s_vtbl, 0));
}

void server::unregister_adaptor(adaptor* a,
								const std::string& name)
{
	DBOOST_CHECK(dbus_connection_unregister_object_path(m_connection.get(), name.c_str()));
    for (auto it = m_adaptors.begin(); it != m_adaptors.end(); ++it) {
    	if (it->second == a) {
    		it = m_adaptors.erase(it);
    	}
    }
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
    auto iter = m_adaptors.find(dbus_message_get_interface(message));
    if (iter == m_adaptors.end()) {
    	clog << "Warning: unknown interface" << dbus_message_get_interface(message) << endl;
    	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    return iter->second->handle_message(connection, message);
}

} // namespace dboost
