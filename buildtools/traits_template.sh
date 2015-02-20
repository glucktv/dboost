#!/bin/sh

#
# print header
#

echo "
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
};"

#
# generate traits
#

for T in "$@"
do

UT=`echo $T | sed 's/.*/\u&/'`

echo "
template <>
struct dbus_traits<DBus${UT}>
{
    static void ref(DBus${UT}* x)
    {
        dbus_${T}_ref(x);
    }
    static void unref(DBus${UT}* x)
    {
    	dbus_${T}_unref(x);
    }
};"

done

#
# print footer
#

echo "
} // namespace dboost

#endif // DBOOST_TRAITS_H
"
