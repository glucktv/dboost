#include <map>
#include <iostream>
#include <cassert>
#include <cstring>
#include <dbus/dbus.h>
#include <serializer.h>
#include "server.h"
#include "exception.h"
#include "monitorable_timer_adaptor.hpp"

namespace dboost_test
{
const char* monitorable_timer_adaptor::INTERFACE_NAME = "org.dboost.monitorable_timer";

monitorable_timer_adaptor::monitorable_timer_adaptor(dboost::server& s)
    : timer_adaptor(s),
      m_server(s)
{
}

monitorable_timer_adaptor::~monitorable_timer_adaptor()
{
}

void monitorable_timer_adaptor::add_object(dboost_test::timer* t, const std::string& name)
{
    std::clog << __FUNCTION__ << " object = " << name << " interface = " << INTERFACE_NAME << std::endl;    auto obj = dynamic_cast<dboost_test::monitorable_timer*>(t);
    if (t == NULL) {
        throw dboost::exception("Wrong type of object passed");
    }
    timer_adaptor::add_object(t, name);
    m_objects[name] = obj;
}

void monitorable_timer_adaptor::remove_object(const std::string& name)
{
    std::clog << __FUNCTION__ << std::endl;
    auto it = m_objects.find(name);
    if (it != m_objects.end()) {
        m_objects.erase(it);
        timer_adaptor::remove_object(name);
    }
}

dboost::dbus_ptr<DBusMessage> monitorable_timer_adaptor::call_set_monitor(dboost_test::monitorable_timer* t, DBusMessage* m)
{
    std::clog << __FUNCTION__ << std::endl;
    assert(t && m);

    
    
    t->set_monitor();
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    
    return result;

}

dboost::dbus_ptr<DBusMessage> monitorable_timer_adaptor::call_reset_monitor(dboost_test::monitorable_timer* t, DBusMessage* m)
{
    std::clog << __FUNCTION__ << std::endl;
    assert(t && m);

    
    
    t->reset_monitor();
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    
    return result;

}

DBusHandlerResult monitorable_timer_adaptor::handle_message(DBusConnection* connection, DBusMessage* message)
{
    std::clog << __FUNCTION__ << std::endl;

    if (!dbus_message_has_interface(message, INTERFACE_NAME)) {
        return timer_adaptor::handle_message(connection, message);
    }

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

    typedef dboost::dbus_ptr<DBusMessage> (monitorable_timer_adaptor::*caller)(monitorable_timer*, DBusMessage*);
    typedef std::map<std::string, caller> caller_table;
    static const caller_table vtbl {
        {"set_monitor", &monitorable_timer_adaptor::call_set_monitor},
        {"reset_monitor", &monitorable_timer_adaptor::call_reset_monitor}
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

