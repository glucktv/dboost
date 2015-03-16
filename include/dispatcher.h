/*
 * watch.h
 *
 *  Created on: 19.02.2015
 *      Author: stas
 */

#ifndef DBOOST_DISPATCHER_H
#define DBOOST_DISPATCHER_H

#include <dbus/dbus.h>

namespace dboost {

class dispatcher
{
public:
	virtual ~dispatcher() {}
	virtual dbus_bool_t add_watch(DBusWatch*) = 0;
	virtual void remove_watch(DBusWatch*) = 0;
	virtual void watch_toggled(DBusWatch*) = 0;
};

} /* namespace dboost */

#endif /* DBOOST_DISPATCHER_H */
