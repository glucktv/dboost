/*
 * boost_dispatcher.cpp
 *
 *  Created on: 11.03.2015
 *      Author: stas
 */

#include <boost_dispatcher.h>
#include <iostream>
#include <memory>

namespace dboost {

using namespace boost::asio;
using namespace std;

boost_dispatcher::boost_dispatcher(boost::asio::io_service& ios, dbus_ptr<DBusConnection> c)
    : m_io_service(ios),
      m_connection(c)
{
}

dbus_bool_t boost_dispatcher::add_watch(DBusWatch* w)
{
    //std::clog << __FUNCTION__ << " fd = " << dbus_watch_get_unix_fd(w)
    //          << " enabled = " << dbus_watch_get_enabled(w)
    //          << " flags = " << dbus_watch_get_flags(w)
    //          << " this = " << (intptr_t)(w)
    //          << std::endl;

    if (!dbus_watch_get_enabled(w)) {
        return true;
    }

    int fd = dbus_watch_get_unix_fd(w);
    auto iter = m_descriptors.find(fd);
    if (iter == m_descriptors.end()) {
        auto result = m_descriptors.insert(
            make_pair(fd, posix::stream_descriptor(m_io_service, fd)));
        iter = result.first;
        dbus_watch_set_data(w, &iter->second, nullptr);
    }

    resubscribe(w);
    //std::clog << __FUNCTION__ << " end" << std::endl;
    return true;
}

void boost_dispatcher::remove_watch(DBusWatch* w)
{
    //std::clog << __FUNCTION__ << std::endl;
    std::unique_ptr<posix::stream_descriptor> d(
        reinterpret_cast<posix::stream_descriptor*>(dbus_watch_get_data(w)));
    assert(d);
    //std::clog << __FUNCTION__ << " end" << std::endl;
}

void boost_dispatcher::watch_toggled(DBusWatch* w)
{
    std::clog << __FUNCTION__ << std::endl;
    if (dbus_watch_get_enabled(w)) {
        add_watch(w);
    }
    else {
        remove_watch(w);
    }
}

void boost_dispatcher::resubscribe(DBusWatch* watch)
{
    //std::clog << __FUNCTION__ << std::endl;
    auto flags = dbus_watch_get_flags(watch);
    posix::stream_descriptor* d =
        reinterpret_cast<posix::stream_descriptor*>(dbus_watch_get_data(watch));
    assert(d);

    using namespace std::placeholders;
    if (flags & DBUS_WATCH_READABLE) {
    //    std::clog << "read some" << std::endl;
        d->async_read_some(null_buffers(),
                           std::bind(&boost_dispatcher::handle_read, this, _1, watch));
    }
    if (flags & DBUS_WATCH_WRITABLE) {
    //    std::clog << "write some" << std::endl;
        d->async_write_some(null_buffers(),
                            std::bind(&boost_dispatcher::handle_write, this, _1, watch));
    }
    //std::clog << __FUNCTION__ << " end" << std::endl;
}

void boost_dispatcher::handle_read(const boost::system::error_code& e, DBusWatch* w)
{
    //std::clog << __FUNCTION__ << std::endl;
    if (e) {
        dbus_watch_handle(w, DBUS_WATCH_ERROR);
    }
    else {
        dbus_watch_handle(w, DBUS_WATCH_READABLE);
        while (dbus_connection_dispatch(m_connection.get()) == DBUS_DISPATCH_DATA_REMAINS);
        resubscribe(w);
    }
}

void boost_dispatcher::handle_write(const boost::system::error_code& e, DBusWatch* w)
{
    //std::clog << __FUNCTION__ << std::endl;
    if (e) {
        dbus_watch_handle(w, DBUS_WATCH_ERROR);
    }
    else {
        dbus_watch_handle(w, DBUS_WATCH_WRITABLE);
        while (dbus_connection_dispatch(m_connection.get()) == DBUS_DISPATCH_DATA_REMAINS);
        resubscribe(w);
    }
}

} /* namespace dboost */

