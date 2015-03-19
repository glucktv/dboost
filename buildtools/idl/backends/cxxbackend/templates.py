from collections import defaultdict

out = defaultdict(dict)

out['AdapterHeader']['head'] = """#ifndef @ig@
#define @ig@
#include <map>
#include <dbus/dbus.h>
#include <dbus_ptr.h>
#include <@suffix@.h>
#include "@interface@.hpp"
@util@
"""

out['AdapterHeader']['interface'] = """\
class @class_name@ : public virtual @inherits@
{
public:
  @class_name@(dboost::server& s);
  virtual ~@class_name@();

  virtual DBusHandlerResult handle_message(DBusConnection* connection, DBusMessage* message) override;

  void add_object(@module_name@::@ancestor@* t, const std::string& name);
  void remove_object(const std::string& name);

private:
  dboost::server& m_server;
  std::map<std::string, @module_name@::@interface@*> m_objects;

  static const char* INTERFACE_NAME;
"""

out['AdapterHeader']['operation'] = """dboost::dbus_ptr<DBusMessage> call_@operation@(@module_name@::@interface@* t, DBusMessage* m);"""

out['AdapterSource']['head'] = """\
#include <map>
#include <iostream>
#include <cassert>
#include <cstring>
#include <dbus/dbus.h>
#include <serializer.h>
#include "server.h"
#include "exception.h"
#include "@class_name@.hpp"
"""

out['AdapterSource']['module_top'] = """\
namespace @module_name@
{
const char* @class_name@::INTERFACE_NAME = "org.dboost.@interface@";

@class_name@::@class_name@(dboost::server& s)
    : m_server(s)
{
    m_server.register_@suffix@(this, @class_name@::INTERFACE_NAME);
}

@class_name@::~@class_name@()
{
    m_server.unregister_@suffix@(this, @class_name@::INTERFACE_NAME);
}

void @class_name@::add_object(@module_name@::@interface@* t, const std::string& name)
{
    std::clog << __FUNCTION__ << std::endl;
    m_server.register_object(name);
    m_objects[name] = t;
}

void @class_name@::remove_object(const std::string& name)
{
    std::clog << __FUNCTION__ << std::endl;
    auto it = m_objects.find(name);
    if (it != m_objects.end()) {
        m_server.unregister_object(name);
        m_objects.erase(it);
    }
}
"""

out['AdapterSource']['module_bottom'] = """\
DBusHandlerResult @class_name@::handle_message(DBusConnection* connection, DBusMessage* message)
{
    std::clog << __FUNCTION__ << std::endl;
    assert(dbus_message_has_interface(message, INTERFACE_NAME));
    auto dst = dbus_message_get_path(message);
    if (dst == nullptr || strlen(dst) == 0) {
        std::cerr << "Unknown message destination" << std::endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    auto obj = m_objects.find(dst);
    if (obj == m_objects.end()) {
        std::cerr << "Destination object " << dst << " not found" << std::endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    auto name = dbus_message_get_member(message);
    if (name == nullptr || strlen(name) == 0) {
        std::cerr << "Unknown method name " << std::endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    typedef dboost::dbus_ptr<DBusMessage> (@class_name@::*caller)(@interface@*, DBusMessage*);
    typedef std::map<std::string, caller> caller_table;
    static const caller_table vtbl {
        @operations@
    };

    auto func = vtbl.find(name);
    if (func == vtbl.end()) {
        std::cerr << "Wrong method name " << name << std::endl;
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    auto result = (this->*func->second)(obj->second, message);

    std::clog << "Reply result" << std::endl;
    dbus_connection_send(connection, result.get(), 0);
    return DBUS_HANDLER_RESULT_HANDLED;
}

}
"""

out['AdapterSource']['operation'] = """\
dboost::dbus_ptr<DBusMessage> @class_name@::call_@operation@(@module_name@::@interface@* t, DBusMessage* m)
{
    std::clog << __FUNCTION__ << std::endl;
    assert(t && m);

    @params_def@
    @params_serialize@
    try {
        @call@;
        dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
        @params_out_serialize@
        return result;
    }
    @exception_catch@
    catch (...) {
        return dboost::dbus_ptr<DBusMessage>();
    }
"""

out['ProxyHeader']['head'] = """\
#ifndef @ig@
#define @ig@
#include <string>
#include <dbus_ptr.h>
#include "@interface@.hpp"
@util@
"""

out['ProxyHeader']['interface'] = """\
class @class_name@: virtual public @inherits@
{
private:
  dboost::dbus_ptr<DBusConnection> m_connection;
  const std::string m_bus_name;
  const std::string m_obj_name;
  static const char* s_ifc_name;
public:
  @class_name@(dboost::dbus_ptr<DBusConnection> conn, const std::string& bus_name, const std::string& obj_name);
"""

out['ProxyHeader']['operation'] = """virtual @rtype@ @id@(@params@)@raises@ override;"""

out['Serializer']['head'] = """\
#ifndef @ig@
#define @ig@

#include "@interface@.hpp"
"""

out['Serializer']['struct'] = """\
template <typename A>
A& serialize(A& a, @struct@& s)
{
    return a @members@;
}
"""

out['ProxySource']['head'] = """\
#include <iostream>
#include <cstring>
#include <dbus/dbus.h>
#include <exception.h>
#include <serializer.h>
#include "@class_name@.hpp"
"""

out['ProxySource']['module'] = """\
namespace @module_name@
{

const char* @class_name@::s_ifc_name = "org.dboost.@interface@";
const int TIMEOUT_MS = 5000;

@class_name@::@class_name@(dboost::dbus_ptr<DBusConnection> conn,
                        const std::string& bus_name, const std::string& obj_name)
    : m_connection(conn),
      m_bus_name(bus_name),
      m_obj_name(obj_name)
{
}
"""

out['ProxySource']['operation'] = """\
@rtype@ @class_name@::@operation@(@params@)@raises@
{
    // create caller (name, arguments)
    dboost::dbus_ptr<DBusMessage> msg(DBOOST_CHECK(dbus_message_new_method_call(m_bus_name.c_str(), m_obj_name.c_str(), s_ifc_name, "@operation@")));
    @params_serialize@;

    // call synchronously
    dboost::error err;
    dboost::dbus_ptr<DBusMessage> reply(dbus_connection_send_with_reply_and_block(m_connection.get(), msg.get(), TIMEOUT_MS, &err));

    // check if there was an error
    if (dbus_error_is_set(&err)) {
        @exceptions_throw@
        throw dboost::exception(dbus_message_get_error_name(reply.get()));
    }

    // unpack output parameters
    @params_out_serialize@;
    return @result@;
}

"""