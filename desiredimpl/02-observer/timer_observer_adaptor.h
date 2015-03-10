/*
 * timer_observer_adaptor.h
 *
 *  Created on: 04.03.2015
 *      Author: stas
 */

#ifndef TIMER_OBSERVER_ADAPTOR_H_
#define TIMER_OBSERVER_ADAPTOR_H_

#include <adaptor.h>
#include <timer.hpp>
#include <string>
#include <map>
#include <dbus_ptr.h>

namespace dboost
{

class server;

}

namespace dboost_test
{

class timer_observer_adaptor: public virtual dboost::adaptor
{
public:
    timer_observer_adaptor(dboost::server& s);
    ~timer_observer_adaptor();

    virtual DBusHandlerResult handle_message(DBusConnection* connection, DBusMessage* message) override;

    void add_object(dboost_test::timer_observer* t, const std::string& name);
    void remove_object(const std::string& name);
    std::string get_object_name(timer_observer* ptr);

private:
    dboost::server& m_server;
    std::map<std::string, dboost_test::timer_observer*> m_objects;

    dboost::dbus_ptr<DBusMessage> call_on_timeout(dboost_test::timer_observer* t, DBusMessage* m);

    static const char* INTERFACE_NAME;

};

} // namespace dboost_test

namespace dboost
{

template <>
struct adaptor_traits<dboost_test::timer_observer>
{
    typedef dboost_test::timer_observer_adaptor type;
    static const char* interface_name;
};

}

#endif /* TIMER_OBSERVER_ADAPTOR_H_ */
