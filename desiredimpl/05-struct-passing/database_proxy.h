/*
 * database_proxy.h
 *
 *  Created on: 03.03.2015
 *      Author: stas
 */

#ifndef DATABASE_PROXY_H_
#define DATABASE_PROXY_H_

#include <database.hpp>
#include <dbus_ptr.h>

namespace dboost_test
{

class database_proxy: public virtual dboost_test::database
{
public:
    database_proxy(dboost::dbus_ptr<DBusConnection> conn,
                   const std::string& bus_name, const std::string& obj_name);
    long add_person(const dboost_test::person& p);
    dboost_test::person find_person_by_id(const long id) throw (dboost_test::id_not_found);
    dboost_test::persons find_by_family(const std::string& fn);

private:
    dboost::dbus_ptr<DBusConnection> m_connection;
    const std::string m_bus_name;
    const std::string m_obj_name;
    static const char* s_ifc_name;
};

} // namespace dboost

#endif /* DATABASE_PROXY_H_ */
