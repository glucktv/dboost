/*
 * server.h
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <vector>
#include <string>
#include <map>
#include <dbus/dbus.h>
#include "dbus_ptr.h"

namespace dboost
{

class adaptor;

class server
{
public:
    server(const std::string& name);
    ~server();

    void register_adaptor(adaptor* a, const std::string& ifcname);
    void unregister_adaptor(adaptor* a, const std::string& ifcname);

    void register_object(const std::string& name);
    void unregister_object(const std::string& name);

    void run();

private:
    std::string m_name;
    dbus_ptr<DBusConnection> m_connection;
    std::map<std::string, adaptor*> m_adaptors;
    static DBusObjectPathVTable s_vtbl;
    static DBusHandlerResult message_func(DBusConnection* connection,
                       DBusMessage* message, void* user_data);
    DBusHandlerResult message_func_impl(DBusConnection* connection,
    	                   DBusMessage* message);

};

} // namespace dboost

#endif /* SERVER_H_ */
