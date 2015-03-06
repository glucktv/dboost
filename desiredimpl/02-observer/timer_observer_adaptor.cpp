/*
 * timer_observer_adaptor.cpp
 *
 *  Created on: 04.03.2015
 *      Author: stas
 */

#include "timer_observer_adaptor.h"
#include "server.h"
#include "exception.h"
#include <dbus/dbus.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <serializer.h>
#include <map>

using namespace std;

namespace dboost_test
{

const char* timer_observer_adaptor::INTERFACE_NAME = "org.dboost.timerobserver";

timer_observer_adaptor::timer_observer_adaptor(dboost::server& s)
    : m_server(s)
{
    m_server.register_adaptor(this, timer_observer_adaptor::INTERFACE_NAME);
}

timer_observer_adaptor::~timer_observer_adaptor()
{
    m_server.unregister_adaptor(this, timer_observer_adaptor::INTERFACE_NAME);
}

DBusHandlerResult
timer_observer_adaptor::handle_message(DBusConnection* connection,
                                       DBusMessage* message)
{
    clog << __FUNCTION__ << endl;
    assert(dbus_message_has_interface(message, INTERFACE_NAME));
    auto dst = dbus_message_get_path(message);
    if (dst == nullptr || strlen(dst) == 0) {
        cerr << "Unknown message destination" << endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    auto obj = m_objects.find(dst);
    if (obj == m_objects.end()) {
        cerr << "Destination object " << dst << " not found" << endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    auto name = dbus_message_get_member(message);
    if (name == nullptr || strlen(name) == 0) {
        cerr << "Unknown method name " << endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    typedef dboost::dbus_ptr<DBusMessage> (timer_observer_adaptor::*caller)(timer_observer*, DBusMessage*);
    typedef map<string, caller> caller_table;
    static const caller_table vtbl {
        { "on_timeout", &timer_observer_adaptor::call_on_timeout },
    };

    auto func = vtbl.find(name);
    if (func == vtbl.end()) {
        cerr << "Wrong method name " << name << endl;
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    auto result = (this->*func->second)(obj->second, message);

    if (result) {
        clog << "Reply result" << endl;
        dbus_connection_send(connection, result.get(), 0);
    }
    return DBUS_HANDLER_RESULT_HANDLED;

}

void
timer_observer_adaptor::add_object(dboost_test::timer_observer* t,
                                   const std::string& name)
{
    clog << __FUNCTION__ << endl;
    m_server.register_object(name);
    m_objects[name] = t;
}

void
timer_observer_adaptor::remove_object(const std::string& name)
{
    clog << __FUNCTION__ << endl;
    auto it = m_objects.find(name);
    if (it != m_objects.end()) {
        m_server.unregister_object(name);
        m_objects.erase(it);
    }
}

dboost::dbus_ptr<DBusMessage>
timer_observer_adaptor::call_on_timeout(dboost_test::timer_observer* t,
                                       DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
    assert(t && m);
    t->on_timeout();
    return dboost::dbus_ptr<DBusMessage>(nullptr);
}

} // namespace dboost_test
