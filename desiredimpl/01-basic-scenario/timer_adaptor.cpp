/*
 * timer_adaptor.cpp
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#include "timer_adaptor.h"
#include "server.h"
#include "exception.h"
#include <dbus/dbus.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <serializer.h>
#include <map>

using namespace std;

namespace dboost
{

const char* adaptor_traits<dboost_test::timer>::interface_name = "org.dboost.timer";

}

namespace dboost_test
{

const char* timer_adaptor::INTERFACE_NAME = dboost::adaptor_traits<dboost_test::timer>::interface_name;

timer_adaptor::timer_adaptor(dboost::server& s)
    : m_server(s)
{
//    m_server.register_adaptor(this, timer_adaptor::INTERFACE_NAME);
}

timer_adaptor::~timer_adaptor()
{
//    m_server.unregister_adaptor(this, timer_adaptor::INTERFACE_NAME);
}

DBusHandlerResult timer_adaptor::handle_message(DBusConnection* connection, DBusMessage* message)
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

    typedef dboost::dbus_ptr<DBusMessage> (timer_adaptor::*caller)(timer*, DBusMessage*);
    typedef map<string, caller> caller_table;
    static const caller_table vtbl {
        { "add_timer", &timer_adaptor::call_add_timer },
        { "remove_timer", &timer_adaptor::call_remove_timer },
    };

    auto func = vtbl.find(name);
    if (func == vtbl.end()) {
        cerr << "Wrong method name " << name << endl;
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    auto result = (this->*func->second)(obj->second, message);

    clog << "Reply result" << endl;
    dbus_connection_send(connection, result.get(), 0);
    return DBUS_HANDLER_RESULT_HANDLED;

}

void timer_adaptor::add_object(dboost_test::timer* t, const std::string& name)
{
    clog << __FUNCTION__ << endl;
    m_objects[name] = t;
}

void timer_adaptor::remove_object(const std::string& name)
{
    clog << __FUNCTION__ << endl;
    auto it = m_objects.find(name);
    if (it != m_objects.end()) {
        m_objects.erase(it);
    }
}

dboost::dbus_ptr<DBusMessage> timer_adaptor::call_add_timer(dboost_test::timer* t, DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
    assert(t && m);
    dboost::iserializer is(m);
    long a0;
    is & a0;
    long r = t->add_timer(a0);
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    dboost::oserializer os(result.get());
    os & r;
    return result;
}

dboost::dbus_ptr<DBusMessage> timer_adaptor::call_remove_timer(dboost_test::timer* t, DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
    assert(t && m);
    dboost::iserializer is(m);
    long a0;
    is & a0;
    t->remove_timer(a0);
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    return result;
}

}
