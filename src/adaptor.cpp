/*
 * adaptor.cpp
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#include "adaptor.h"
#include <iostream>

using namespace std;

namespace dboost
{

DBusHandlerResult adaptor::handle_message(DBusConnection*, DBusMessage*)
{
    clog << "Warning: message is not handled" << endl;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

} // namespace dboost
