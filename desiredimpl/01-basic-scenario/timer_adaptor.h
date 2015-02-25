/*
 * timer_adaptor.h
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#ifndef TIMER_ADAPTOR_H_
#define TIMER_ADAPTOR_H_

#include "adaptor.h"
#include "timer.hpp"

namespace dboost
{

class server;

class timer_adaptor: public virtual dboost::adaptor
{
public:
    timer_adaptor(server& s);
    virtual ~timer_adaptor();
    virtual DBusHandlerResult handle_message(DBusConnection* connection, DBusMessage* message) override;

    void add_object(dboost_test::timer* t, const std::string& name);
    void remove_object(const std::string& name);

private:
    server& m_server;
    std::map<std::string, dboost_test::timer*> m_objects;

    dbus_ptr<DBusMessage> call_add_timer(dboost_test::timer* t, DBusMessage* m);
    dbus_ptr<DBusMessage> call_remove_timer(dboost_test::timer* t, DBusMessage* m);

    static const char* INTERFACE_NAME;
};

}

#endif /* TIMER_ADAPTOR_H_ */
