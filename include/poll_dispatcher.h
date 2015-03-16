/*
 * poll_dispatcher.h
 *
 *  Created on: 12.03.2015
 *      Author: stas
 */

#ifndef POLL_DISPATCHER_H_
#define POLL_DISPATCHER_H_

#include "dispatcher.h"
#include <dbus/dbus.h>
#include <sys/poll.h>

namespace dboost
{

/* The implementation is completely taken from:
 * http://lists.freedesktop.org/archives/dbus/2007-October/008859.html
 */

static const int MAX_WATCHES = 100;

class poll_dispatcher: public dispatcher
{
public:
    poll_dispatcher(DBusConnection *c): max_i(0), conn(c) {}
    virtual dbus_bool_t add_watch(DBusWatch* watch);
    virtual void remove_watch(DBusWatch* watch);
    virtual void watch_toggled(DBusWatch* watch);

    void run();

private:
    struct pollfd pollfds[MAX_WATCHES];
    DBusWatch *watches[MAX_WATCHES];
    int max_i;
    DBusConnection *conn;

    void fd_handler(short events, DBusWatch* watch);
};

} /* namespace dboost */

#endif /* POLL_DISPATCHER_H_ */
