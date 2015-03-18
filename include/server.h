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
class dispatcher;

class server
{
public:
    server(dbus_ptr<DBusConnection> connection, const std::string& name);
    ~server();

    void register_object(adaptor* a, const std::string& name);
    void unregister_object(adaptor* a, const std::string& name);

    void run();
    void set_dispatcher(dispatcher* disp);

private:
    const std::string m_name;
    dbus_ptr<DBusConnection> m_connection;
    std::map<std::string, adaptor*> m_adaptors;
    static DBusObjectPathVTable s_vtbl;
    static DBusHandlerResult message_func(DBusConnection* connection,
                       DBusMessage* message, void* user_data);
    DBusHandlerResult message_func_impl(DBusConnection* connection,
    	                   DBusMessage* message);

    static dbus_bool_t add_watch(DBusWatch* watch, void* data);
    static void remove_watch(DBusWatch* watch, void* data);
    static void watch_toggled(DBusWatch* watch, void* data);

//    void register_adaptor(adaptor* a);
//    void unregister_adaptor(adaptor* a);
//
//    void register_object(const std::string& name);
//    void unregister_object(const std::string& name);


};

} // namespace dboost

#endif /* SERVER_H_ */
