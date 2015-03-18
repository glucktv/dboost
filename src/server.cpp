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

server::server(dbus_ptr<DBusConnection> connection, const std::string& name)
    : m_name(name),
      m_connection(connection)
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

void server::register_object(adaptor* a, const std::string& name)
{
    assert(a != nullptr);
    clog << __FUNCTION__ << " object = " << name << endl;
    DBOOST_CHECK(dbus_connection_register_object_path(m_connection.get(), name.c_str(), &s_vtbl, this));
    m_adaptors[name] = a;
}

void server::unregister_object(adaptor* a, const std::string& name)
{
    assert(a != nullptr);
    clog << __FUNCTION__ << " object = " << name << endl;
    auto iter = m_adaptors.find(name);
    if (iter == m_adaptors.end()) {
        cerr << "Trying to unregister inexistent interface " << name << endl;
    }
    else {
        m_adaptors.erase(iter);
        DBOOST_CHECK(dbus_connection_unregister_object_path(m_connection.get(), name.c_str()));
    }
}

void
server::set_dispatcher(dispatcher* disp)
{
    DBOOST_CHECK(dbus_connection_set_watch_functions(m_connection.get(),
                                                     &server::add_watch,
                                                     &server::remove_watch,
                                                     &server::watch_toggled,
                                                     disp,
                                                     nullptr));
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

    for (auto iter = m_adaptors.find(dbus_message_get_path(message));
         iter != m_adaptors.end();
         iter = m_adaptors.find(dbus_message_get_path(message))) {
        if (iter->second->handle_message(connection, message) == DBUS_HANDLER_RESULT_HANDLED) {
            return DBUS_HANDLER_RESULT_HANDLED;
        }
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

void server::run()
{
    while (dbus_connection_read_write_dispatch (m_connection.get(), -1));
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

