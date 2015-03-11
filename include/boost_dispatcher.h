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

namespace dboost {

class boost_dispatcher: public dispatcher
{
public:
    boost_dispatcher(boost::asio::io_service& ios);

    virtual dbus_bool_t add_watch(DBusWatch*);
    virtual void remove_watch(DBusWatch*);
    virtual void watch_toggled(DBusWatch*);

private:
    boost::asio::io_service& m_io_service;
    void resubscribe(DBusWatch* watch);
    void handle_read(const boost::system::error_code& e, DBusWatch* w);
    void handle_write(const boost::system::error_code& e, DBusWatch* w);
};

} /* namespace dboost */

#endif /* BOOST_DISPATCHER_H_ */
