/*
 * adaptor.h
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#ifndef ADAPTOR_H_
#define ADAPTOR_H_

#include "server.h"
#include <dbus/dbus.h>

namespace dboost
{

class adaptor
{
public:
    virtual ~adaptor() {}
    virtual DBusHandlerResult handle_message(DBusConnection* connection, DBusMessage* message);
};

} // namespace dboost

#endif /* ADAPTOR_H_ */
