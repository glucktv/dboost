/*
 * utils.h
 *
 *  Created on: 26.02.2015
 *      Author: stas
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <dbus_ptr.h>
#include <dbus/dbus.h>

namespace dboost
{

dbus_ptr<DBusConnection> create_connection();
std::string as_string(int dbus_type);

}

#endif /* UTILS_H_ */
