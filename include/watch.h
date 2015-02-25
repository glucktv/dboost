/*
 * watch.h
 *
 *  Created on: 19.02.2015
 *      Author: stas
 */

#ifndef DBOOST_WATCH_H
#define DBOOST_WATCH_H

namespace dboost {

class watch
{
public:
	virtual ~watch() {}
	virtual dbus_bool_t add_watch(DBusWatch*) = 0;
	virtual void remove_watch(DBusWatch*) = 0;
	virtual void watch_toggled(DBusWatch*) = 0;
};

} /* namespace dboost */

#endif /* DBOOST_WATCH_H */
