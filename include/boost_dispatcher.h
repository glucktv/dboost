/*
 * boost_watch.h
 *
 *  Created on: 19.02.2015
 *      Author: stas
 */

#ifndef BOOST_DISPATCHER_H_
#define BOOST_DISPATCHER_H_

#include "dispatcher.h"
#include <boost/asio.hpp>
#include <dbus_ptr.h>

namespace dboost {

class boost_dispatcher: public dispatcher
{
public:
    boost_dispatcher(boost::asio::io_service& ios, dbus_ptr<DBusConnection> c);

    virtual dbus_bool_t add_watch(DBusWatch*);
    virtual void remove_watch(DBusWatch*);
    virtual void watch_toggled(DBusWatch*);

private:
    boost::asio::io_service& m_io_service;
    dbus_ptr<DBusConnection> m_connection;
    std::map<int, boost::asio::posix::stream_descriptor> m_descriptors;

    void resubscribe(DBusWatch* watch);
    void handle_read(const boost::system::error_code& e, DBusWatch* w);
    void handle_write(const boost::system::error_code& e, DBusWatch* w);
};

} /* namespace dboost */

#endif /* BOOST_DISPATCHER_H_ */
