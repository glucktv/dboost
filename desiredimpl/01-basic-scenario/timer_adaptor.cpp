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

using namespace std;

namespace dboost
{

const char* timer_adaptor::INTERFACE_NAME = "org.dboost.timer";

timer_adaptor::timer_adaptor(server& s)
	: m_server(s)
{
    m_server.register_adaptor(this, timer_adaptor::INTERFACE_NAME);
}

timer_adaptor::~timer_adaptor()
{
    m_server.unregister_adaptor(this, timer_adaptor::INTERFACE_NAME);
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

	dbus_ptr<DBusMessage> result;

#warning To automatically generate!

    if (strcmp(name, "add_timer") == 0) {
        result = call_add_timer(obj->second, message);
    }
    else if (strcmp(name, "remove_timer") == 0) {
        result = call_remove_timer(obj->second, message);
    }
    else {
        cerr << "Wrong method name " << name << endl;
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    clog << "Reply result" << endl;
    dbus_connection_send(connection, result.get(), 0);
    return DBUS_HANDLER_RESULT_HANDLED;

#warning End automatically generate
}

void timer_adaptor::add_object(dboost_test::timer* t, const std::string& name)
{
    clog << __FUNCTION__ << endl;
    m_server.register_object(name);
    m_objects[name] = t;
}

void timer_adaptor::remove_object(const std::string& name)
{
    clog << __FUNCTION__ << endl;
    auto it = m_objects.find(name);
    if (it != m_objects.end()) {
        m_server.unregister_object(name);
        m_objects.erase(it);
    }
}

dbus_ptr<DBusMessage> timer_adaptor::call_add_timer(dboost_test::timer* t, DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
	assert(t && m);
	DBusMessageIter it; // no destroying code needed

	DBOOST_CHECK(dbus_message_iter_init(m, &it));


    if (dbus_message_iter_get_arg_type(&it) != DBUS_TYPE_INT64)	{
        cerr << "Wrong argument" << endl;
	    return dbus_ptr<DBusMessage>(dbus_message_new_error(m, DBUS_ERROR_INVALID_ARGS, "Wrong argument"), true);
	}

    long a0;
	dbus_message_iter_get_basic(&it, &a0);

	long r = t->add_timer(a0);

	dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
	dbus_message_iter_init_append(result.get(), &it);
	DBOOST_CHECK(dbus_message_iter_append_basic(&it, DBUS_TYPE_INT64, &r));

	return result;
}

dbus_ptr<DBusMessage> timer_adaptor::call_remove_timer(dboost_test::timer* t, DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
	assert(t && m);
	DBusMessageIter it; // no destroying code needed
	DBOOST_CHECK(dbus_message_iter_init(m, &it));
    if (dbus_message_iter_get_arg_type(&it) != DBUS_TYPE_INT64) {
        cerr << "Wrong argument" << endl;
        return dbus_ptr<DBusMessage>(dbus_message_new_error(m, DBUS_ERROR_INVALID_ARGS, "Wrong argument"), true);
    }

	long a0;
	dbus_message_iter_get_basic(&it, &a0);
	t->remove_timer(a0);

	dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));

	return result;
}

}
