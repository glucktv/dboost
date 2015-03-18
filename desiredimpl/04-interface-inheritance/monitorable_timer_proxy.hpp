#ifndef MONITORABLE_TIMER_PROXY_HPP
#define MONITORABLE_TIMER_PROXY_HPP
#include <string>
#include <dbus_ptr.h>
#include "timer_proxy.hpp"
#include "monitorable_timer.hpp"


namespace dboost_test
{

class monitorable_timer_proxy: virtual public dboost_test::monitorable_timer,
                               virtual public dboost_test::timer_proxy
{
private:
    dboost::dbus_ptr<DBusConnection> m_connection;
    const std::string m_bus_name;
    const std::string m_obj_name;
    static const char* s_ifc_name;
public:
    monitorable_timer_proxy(dboost::dbus_ptr<DBusConnection> conn, const std::string& bus_name, const std::string& obj_name);

    virtual void set_monitor() override;
    virtual void reset_monitor() override;
};

}

#endif // MONITORABLE_TIMER_PROXY_HPP
