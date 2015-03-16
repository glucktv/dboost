/*
 * poll_dispatcher.cpp
 *
 *  Created on: 12.03.2015
 *      Author: stas
 */

#include "poll_dispatcher.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dbus/dbus.h>

namespace dboost
{

dbus_bool_t poll_dispatcher::add_watch(DBusWatch* watch)
{
    short cond = POLLHUP | POLLERR;
    int fd;
    unsigned int flags;

    fd = dbus_watch_get_unix_fd(watch);
    flags = dbus_watch_get_flags(watch);
    printf("add watch %p fd %d flags %d enabled %d\n",
           (void*)watch, fd, flags,
           dbus_watch_get_enabled(watch));

    if (flags & DBUS_WATCH_READABLE)
            cond |= POLLIN;
    if (flags & DBUS_WATCH_WRITABLE)
            cond |= POLLOUT;

    pollfds[max_i].fd = fd;
    pollfds[max_i].events = cond;
    watches[max_i] = watch;
    ++max_i;

    return 1;
}

void poll_dispatcher::remove_watch(DBusWatch* watch)
{
    int i, found = 0;

    printf("remove watch %p\n", (void*)watch);
    for (i = 0; i <= max_i; ++i) {
            if (watches[i] == watch) {
                    found = 1;
                    break;
            }
    }
    if (!found) {
            printf("watch %p not found\n", (void*)watch);
            return;
    }

    memset(&pollfds[i], 0, sizeof(pollfds[i]));
    watches[i] = NULL;

    if (i == max_i && max_i > 0) --max_i;
}

void poll_dispatcher::watch_toggled(DBusWatch* watch)
{
    int fd = dbus_watch_get_unix_fd(watch);
    int flags = dbus_watch_get_flags(watch);
    printf("watch toggled %p fd %d flags %d enabled %d\n",
           (void*)watch, fd, flags,
           dbus_watch_get_enabled(watch));

}

void poll_dispatcher::run()
{
    while (1) {
            struct pollfd fds[MAX_WATCHES];
            DBusWatch *watch[MAX_WATCHES];
            int nfds, i;

            for (nfds = i = 0; i < max_i; ++i) {
                    if (pollfds[i].fd == 0 ||
                        !dbus_watch_get_enabled(watches[i])) {
                        printf("*** Ignore watch %p\n", watches[i]);
                        continue;
                    }
                    printf("*** Polling watch %p\n", watches[i]);
                    fds[nfds].fd = pollfds[i].fd;
                    fds[nfds].events = pollfds[i].events;
                    fds[nfds].revents = 0;
                    watch[nfds] = watches[i];
                    ++nfds;
            }

            if (poll(fds, nfds, -1) <= 0) {
                    perror("poll");
                    break;
            }

            for (i = 0; i < nfds; ++i) {
                    if (fds[i].revents) {
                            fd_handler(fds[i].revents, watch[i]);
                    }
            }
    }
}

void poll_dispatcher::fd_handler(short events, DBusWatch* watch)
{
    unsigned int flags = 0;

    if (events & POLLIN)
            flags |= DBUS_WATCH_READABLE;
    if (events & POLLOUT)
            flags |= DBUS_WATCH_WRITABLE;
    if (events & POLLHUP)
            flags |= DBUS_WATCH_HANGUP;
    if (events & POLLERR)
            flags |= DBUS_WATCH_ERROR;

    while (!dbus_watch_handle(watch, flags)) {
            printf("dbus_watch_handle needs more memory\n");
            sleep(1);
    }

    dbus_connection_ref(conn);
    while (dbus_connection_dispatch(conn) == DBUS_DISPATCH_DATA_REMAINS);
    dbus_connection_unref(conn);
}

} /* namespace dboost */
