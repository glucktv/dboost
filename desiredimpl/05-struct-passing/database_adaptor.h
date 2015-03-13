/*
 * database_adaptor.h
 *
 *  Created on: 02.03.2015
 *      Author: stas
 */

#ifndef DATABASE_ADAPTOR_H_
#define DATABASE_ADAPTOR_H_

#include "adaptor.h"
#include "server.h"
#include "exception.h"
#include <dbus/dbus.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <serializer.h>
#include <map>
#include <database_utils.h>

namespace dboost
{

class server;

}

namespace dboost_test
{

class database_adaptor: virtual public dboost::adaptor
{
public:
    database_adaptor(dboost::server& s);
    virtual ~database_adaptor();

    virtual DBusHandlerResult handle_message(DBusConnection* connection, DBusMessage* message) override;
    void add_object(dboost_test::database* t, const std::string& name);
    void remove_object(const std::string& name);

private:
    dboost::server& m_server;
    std::map<std::string, dboost_test::database*> m_objects;

    dboost::dbus_ptr<DBusMessage> call_add_person(dboost_test::database* t, DBusMessage* m);
    dboost::dbus_ptr<DBusMessage> call_find_person_by_id(dboost_test::database* t, DBusMessage* m);
    dboost::dbus_ptr<DBusMessage> call_find_by_family(dboost_test::database* t, DBusMessage* m);

    static const char* INTERFACE_NAME;

};

}

namespace dboost
{

template <>
struct adaptor_traits<dboost_test::database>
{
    typedef dboost_test::database_adaptor type;
    static const char* interface_name;
};

} // namespace dboost

#endif /* DATABASE_ADAPTOR_H_ */
