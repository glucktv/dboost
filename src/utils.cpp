/*
 * utils.cpp
 *
 *  Created on: 26.02.2015
 *      Author: stas
 */

#include <utils.h>
#include <exception.h>

namespace dboost
{

dbus_ptr<DBusConnection> create_connection()
{
    error err;
    dbus_ptr<DBusConnection> s(dbus_bus_get(DBUS_BUS_SESSION, &err));
    DBOOST_CHECK_WITH_ERR(s, err);
    return s;
}

std::string as_string(int dbus_type)
{
    char c = static_cast<int>(dbus_type);
    return std::string(1, c);
}

}
