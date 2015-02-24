
#ifndef DBOOST_TRAITS_H
#define DBOOST_TRAITS_H

#include <dbus/dbus.h>

namespace dboost
{

template <class T>
struct dbus_traits
{
    static void ref(T*);
    static void unref(T*);
};

template <>
struct dbus_traits<DBusConnection>
{
    static void ref(DBusConnection* x)
    {
        dbus_connection_ref(x);
    }
    static void unref(DBusConnection* x)
    {
    	dbus_connection_unref(x);
    }
};

template <>
struct dbus_traits<DBusServer>
{
    static void ref(DBusServer* x)
    {
        dbus_server_ref(x);
    }
    static void unref(DBusServer* x)
    {
    	dbus_server_unref(x);
    }
};

template <>
struct dbus_traits<DBusMessage>
{
    static void ref(DBusMessage* x)
    {
        dbus_message_ref(x);
    }
    static void unref(DBusMessage* x)
    {
    	dbus_message_unref(x);
    }
};


} // namespace dboost

#endif // DBOOST_TRAITS_H

