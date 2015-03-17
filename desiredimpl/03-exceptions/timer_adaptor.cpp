#include <map>
#include <iostream>
#include <cassert>
#include <cstring>
#include <dbus/dbus.h>
#include <serializer.h>
#include "server.h"
#include "exception.h"
#include "timer_adaptor.hpp"

namespace dboost_test
{
const char* timer_adaptor::INTERFACE_NAME = "org.dboost.timer";

timer_adaptor::timer_adaptor(dboost::server& s)
    : m_server(s)
{
    m_server.register_adaptor(this, timer_adaptor::INTERFACE_NAME);
}

timer_adaptor::~timer_adaptor()
{
    m_server.unregister_adaptor(this, timer_adaptor::INTERFACE_NAME);
}

void timer_adaptor::add_object(dboost_test::timer* t, const std::string& name)
{
    std::clog << __FUNCTION__ << std::endl;
    m_server.register_object(name);
    m_objects[name] = t;
}

void timer_adaptor::remove_object(const std::string& name)
{
    std::clog << __FUNCTION__ << std::endl;
    auto it = m_objects.find(name);
    if (it != m_objects.end()) {
        m_server.unregister_object(name);
        m_objects.erase(it);
    }
}

dboost::dbus_ptr<DBusMessage> timer_adaptor::call_add_timer(dboost_test::timer* t, DBusMessage* m)
{
    std::clog << __FUNCTION__ << std::endl;
    assert(t && m);

    long param0;
    dboost::iserializer is(m);
    is & param0;
    try {
        long r = t->add_timer(param0);
        dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
        dboost::oserializer os(result.get());
        os & r ;
        return result;
    }
    catch (const dboost_test::wrong_timeout_exception& e) {
        dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_error(m, "org.dboost.dboost_test.wrong_timeout_exception", e.what())));
        return result;
    }
}

dboost::dbus_ptr<DBusMessage> timer_adaptor::call_remove_timer(dboost_test::timer* t, DBusMessage* m)
{
    std::clog << __FUNCTION__ << std::endl;
    assert(t && m);

    long param0;
    dboost::iserializer is(m);
    is & param0;
    t->remove_timer(param0);
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    
    return result;

}

DBusHandlerResult timer_adaptor::handle_message(DBusConnection* connection, DBusMessage* message)
{
    std::clog << __FUNCTION__ << std::endl;
    assert(dbus_message_has_interface(message, INTERFACE_NAME));
    auto dst = dbus_message_get_path(message);
    if (dst == nullptr || strlen(dst) == 0) {
        std::cerr << "Unknown message destination" << std::endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    auto obj = m_objects.find(dst);
    if (obj == m_objects.end()) {
        std::cerr << "Destination object " << dst << " not found" << std::endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    auto name = dbus_message_get_member(message);
    if (name == nullptr || strlen(name) == 0) {
        std::cerr << "Unknown method name " << std::endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    typedef dboost::dbus_ptr<DBusMessage> (timer_adaptor::*caller)(timer*, DBusMessage*);
    typedef std::map<std::string, caller> caller_table;
    static const caller_table vtbl {
        {"add_timer", &timer_adaptor::call_add_timer},
        {"remove_timer", &timer_adaptor::call_remove_timer}
    };

    auto func = vtbl.find(name);
    if (func == vtbl.end()) {
        std::cerr << "Wrong method name " << name << std::endl;
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    auto result = (this->*func->second)(obj->second, message);

    std::clog << "Reply result" << std::endl;
    dbus_connection_send(connection, result.get(), 0);
    return DBUS_HANDLER_RESULT_HANDLED;
}

}

