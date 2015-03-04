/*
 * timer_observer_proxy.h
 *
 *  Created on: 04.03.2015
 *      Author: stas
 */

#ifndef TIMER_OBSERVER_PROXY_H_
#define TIMER_OBSERVER_PROXY_H_

#include <timer.hpp>
#include <dbus_ptr.h>

namespace dboost_test
{

class timer_observer_proxy: public virtual timer_observer
{
public:
    timer_observer_proxy(dboost::dbus_ptr<DBusConnection> conn, const std::string& bus_name, const std::string& obj_name);
    void on_timeout() override;

private:
    dboost::dbus_ptr<DBusConnection> m_connection;
    const std::string m_bus_name;
    const std::string m_obj_name;
    static const char* s_ifc_name;
};

} // namespace dboost_test

#endif /* TIMER_OBSERVER_PROXY_H_ */
