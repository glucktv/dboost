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
#include <memory>
#include <dbus/dbus.h>
#include "dbus_ptr.h"
#include <adaptor.h>
#include <iostream>
#include <exception.h>
#include <utility>

namespace dboost
{

//class adaptor;

class server
{
public:
    server(const std::string& name);
    ~server();

    template <typename T>
    void register_object(T* obj, const char* name);
    template <typename T>
    void unregister_object(T*, const char* name);
    template <typename T>
    std::string get_object_name(T* obj);

    void run();
    dbus_ptr<DBusConnection> get_connection();
    std::string get_unique_name();

private:
    const std::string m_name;
    dbus_ptr<DBusConnection> m_connection;
    std::map<std::string, std::unique_ptr<adaptor>> m_adaptors;
    static DBusObjectPathVTable s_vtbl;
    static DBusHandlerResult message_func(DBusConnection* connection,
        DBusMessage* message, void* user_data);
    DBusHandlerResult message_func_impl(DBusConnection* connection,
    	DBusMessage* message);

};

template <typename T>
void server::register_object(T* obj, const char* name)
{
    std::clog << __FUNCTION__ << " name = " << name << std::endl;
    typedef typename adaptor_traits<T>::type type;
    auto iter = m_adaptors.find(adaptor_traits<T>::interface_name);
    if (iter == m_adaptors.end()) {
        iter = m_adaptors.insert(
                   make_pair(adaptor_traits<T>::interface_name,
                             std::unique_ptr<adaptor>(new type(*this)))).first;
    }
    DBOOST_CHECK(dbus_connection_register_object_path(m_connection.get(), name, &s_vtbl, this));
    dynamic_cast<type*>(iter->second.get())->add_object(obj, name);
}

template <typename T>
void server::unregister_object(T*, const char* name)
{
    std::clog << __FUNCTION__ << " name = " << name << std::endl;
    DBOOST_CHECK(dbus_connection_unregister_object_path(m_connection.get(), name));
    typedef typename adaptor_traits<T>::type type;
    auto iter = m_adaptors.find(adaptor_traits<T>::interface_name);
    if (iter != m_adaptors.end()) {
        dynamic_cast<type*>(iter->second.get())->remove_object(name);
    }
}

template <typename T>
std::string server::get_object_name(T* obj)
{
    typedef typename adaptor_traits<T>::type type;
    for (auto& ap: m_adaptors) {
        type* adaptor = dynamic_cast<type*>(ap.second.get());
        if (adaptor != nullptr) {
            return adaptor->get_object_name(obj);
        }
    }
    throw exception("Type not found");
}


} // namespace dboost

#endif /* SERVER_H_ */
