/*
 * timer_observer_adaptor.h
 *
 *  Created on: 04.03.2015
 *      Author: stas
 */

#ifndef TIMER_OBSERVER_ADAPTOR_H_
#define TIMER_OBSERVER_ADAPTOR_H_

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

private:
    dboost::server& m_server;
    std::map<std::string, dboost_test::timer_observer*> m_objects;

    dboost::dbus_ptr<DBusMessage> call_on_timeout(dboost_test::timer_observer* t, DBusMessage* m);

    static const char* INTERFACE_NAME;

};

} // namespace dboost_test

#endif /* TIMER_OBSERVER_ADAPTOR_H_ */
