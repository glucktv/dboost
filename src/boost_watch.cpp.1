/*
 * boost_watch.cpp
 *
 *  Created on: 19.02.2015
 *      Author: stas
 */

#include "boost_watch.h"
#include <iostream>
#include <boost/asio.hpp>
#include <dbus/dbus.h>

namespace dboost {

boost_watch::boost_watch()
{
}

void boost_watch::resubscribe()
{
    std::clog << __FUNCTION__ << std::endl;
    auto flags = dbus_watch_get_flags(watch);
    auto iter = find_descriptor(watch);
    assert(iter != m_descriptors.end());

    using namespace std::placeholders;
    if (flags & DBUS_WATCH_READABLE) {
      iter->async_read_some(boost::asio::null_buffers(),
                           std::bind(&server::handle_read, this, watch, _1));
    }
    if (flags & DBUS_WATCH_WRITABLE) {
      iter->async_write_some(boost::asio::null_buffers(),
                            std::bind(&server::handle_write, this, watch, _1));

    }
}

} /* namespace dboost */
