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
#include <dispatcher.h>

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
    clog << __FUNCTION__ << endl;
    auto s = reinterpret_cast<server*>(user_data);
    assert(s != nullptr);
    return s->message_func_impl(connection, message);
}

void server::set_dispatcher(dispatcher* w)
{
    DBOOST_CHECK(dbus_connection_set_watch_functions(m_connection.get(),
                                                     &server::add_watch,
                                                     &server::remove_watch,
                                                     &server::watch_toggled,
                                                     w,
                                                     nullptr));
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
    while (dbus_connection_read_write_dispatch (m_connection.get(), -1)) clog << __FUNCTION__ << endl;
    clog << __FUNCTION__ << " end!" << endl;
}

dbus_bool_t server::add_watch(DBusWatch* w, void* data)
{
    dispatcher* d = reinterpret_cast<dispatcher*>(data);
    assert(d != nullptr);
    return d->add_watch(w);
}

void server::remove_watch(DBusWatch* w, void* data)
{
    dispatcher* d = reinterpret_cast<dispatcher*>(data);
    assert(d != nullptr);
    d->remove_watch(w);
}

void server::watch_toggled(DBusWatch* w, void* data)
{
    dispatcher* d = reinterpret_cast<dispatcher*>(data);
    assert(d != nullptr);
    d->watch_toggled(w);
}

} // namespace dboost

