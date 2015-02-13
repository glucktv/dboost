#ifndef DBOOST_PTRUTILS_H
#define DBOOST_PTRUTILS_H

#include <boost/checked_delete.h>

namespace boost { // otherwise scoped_ptr won't find deleters

template <>
void checked_delete<DBusMessage>(DBusMessage* msg)
{
  dbus_message_unref(msg);
}

template <>
void checked_delete<DBusPendingCall>(DBusPendingCall* pc)
{
  dbus_pending_call_unref(pc);
}

};

#endif