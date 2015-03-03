/*
 * database_proxy.cpp
 *
 *  Created on: 03.03.2015
 *      Author: stas
 */

#include "database_proxy.h"
#include <dbus/dbus.h>
#include <exception.h>
#include <iostream>
#include <serializer.h>
#include <database_utils.h>

namespace dboost_test
{

const char* database_proxy::s_ifc_name = "org.dboost.database";
const int TIMEOUT_MS = 5000;

database_proxy::database_proxy(dboost::dbus_ptr<DBusConnection> conn,
                               const std::string& bus_name, const std::string& obj_name)
    : m_connection(conn),
      m_bus_name(bus_name),
      m_obj_name(obj_name)
{
}

long database_proxy::add_person(const dboost_test::person& a0)
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "add_person")));
    dboost::oserializer os(msg.get());
    os & a0;

    // call synchronously
    dboost::error err;
    dboost::dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

    // check if there was an error
    DBOOST_CHECK_WITH_ERR(reply, err);
    if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
        throw dboost::exception(dbus_message_get_error_name(reply.get()));
    }

    // unpack output parameters
    dboost::iserializer is(reply.get());
    long r;
    is & r;
    return r;

}

dboost_test::person database_proxy::find_person_by_id(const long a0)
        throw(dboost_test::id_not_found)
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "find_person_by_id")));
    dboost::oserializer os(msg.get());
    os & a0;

    // call synchronously
    dboost::error err;
    dboost::dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

    // check if there was an error
    DBOOST_CHECK_WITH_ERR(reply, err);
    if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
        throw dboost::exception(dbus_message_get_error_name(reply.get()));
    }

    // unpack output parameters
    dboost::iserializer is(reply.get());
    dboost_test::person r;
    is & r;
    return r;
}

dboost_test::persons database_proxy::find_by_family(const std::string& a0)
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "find_by_family")));
    dboost::oserializer os(msg.get());
    os & a0;

    // call synchronously
    dboost::error err;
    dboost::dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

    // check if there was an error
    DBOOST_CHECK_WITH_ERR(reply, err);
    if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
        throw dboost::exception(dbus_message_get_error_name(reply.get()));
    }

    // unpack output parameters
    dboost::iserializer is(reply.get());
    dboost_test::persons r;
    is & r;
    return r;
}

} // namespace dboost
