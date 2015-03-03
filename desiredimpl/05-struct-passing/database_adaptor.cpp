/*
 * database_adaptor.cpp
 *
 *  Created on: 02.03.2015
 *      Author: stas
 */

#include "database_adaptor.h"
#include "server.h"
#include "exception.h"
#include <dbus/dbus.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <serializer.h>
#include <database_utils.h>
#include <map>

using namespace std;

namespace dboost_test
{

const char* database_adaptor::INTERFACE_NAME = "org.dboost.database";

database_adaptor::database_adaptor(dboost::server& s)
    : m_server(s)
{
    m_server.register_adaptor(this, database_adaptor::INTERFACE_NAME);
}

database_adaptor::~database_adaptor()
{
    m_server.unregister_adaptor(this, database_adaptor::INTERFACE_NAME);
}

DBusHandlerResult
database_adaptor::handle_message(DBusConnection* connection,
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

    typedef dboost::dbus_ptr<DBusMessage> (database_adaptor::*caller)(database*, DBusMessage*);
    typedef map<string, caller> caller_table;
    static const caller_table vtbl {
        { "add_person", &database_adaptor::call_add_person },
        { "find_person_by_id", &database_adaptor::call_find_person_by_id },
        //{ "find_by_family", &database_adaptor::call_find_by_family }
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

void
database_adaptor::add_object(dboost_test::database* t, const std::string& name)
{
    clog << __FUNCTION__ << endl;
    m_server.register_object(name);
    m_objects[name] = t;
}

void
database_adaptor::remove_object(const std::string& name)
{
    clog << __FUNCTION__ << endl;
    auto it = m_objects.find(name);
    if (it != m_objects.end()) {
        m_server.unregister_object(name);
        m_objects.erase(it);
    }
}

dboost::dbus_ptr<DBusMessage>
database_adaptor::call_add_person(dboost_test::database* t, DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
    assert(t && m);
    dboost::iserializer is(m);
    person a0;
    is & a0;
    long r = t->add_person(a0);
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    dboost::oserializer os(result.get());
    os & r;
    return result;
}

dboost::dbus_ptr<DBusMessage>
database_adaptor::call_find_person_by_id(dboost_test::database* t,
                                         DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
    assert(t && m);
    dboost::iserializer is(m);
    long a0;
    is & a0;
    person r = t->find_person_by_id(a0);
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    dboost::oserializer os(result.get());
    os & r;
    return result;
}
#if 0
dboost::dbus_ptr<DBusMessage>
database_adaptor::call_find_by_family(dboost_test::database* t, DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
    assert(t && m);
    dboost::iserializer is(m);
    std::string a0;
    is & a0;
    persons r = t->find_by_family(a0);
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    dboost::oserializer os(result.get());
    os & r;
    return result;
}
#endif
} // namespace dboost
