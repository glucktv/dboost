/*
 * watch.cpp
 *
 *  Created on: 19.02.2015
 *      Author: stas
 */

#include "watch.h"
#include <cassert>

namespace dboost {

dbus_bool_t watch::handle_add_watch(DBusWatch* w, void* data)
{
	std::clog << __FUNCTION__ << std::endl;
	assert(watch != nullptr);
	watch* self = reinterpret_cast<watch*>(data);
	assert(self != nullptr);
	return self->add_watch(w);
}

void watch::handle_remove_watch(DBusWatch* w, void* data)
{
	std::clog << __FUNCTION__ << std::endl;
	assert(watch != nullptr);
	watch* self = reinterpret_cast<watch*>(data);
	assert(self != nullptr);
	self->remove_watch(w);
}

void watch::handle_watch_toggled(DBusWatch* w, void* data)
{
	std::clog << __FUNCTION__ << std::endl;
	assert(watch != nullptr);
	watch* self = reinterpret_cast<server*>(data);
	assert(self != nullptr);
	self->watch_toggled(w);
}

} /* namespace dboost */
