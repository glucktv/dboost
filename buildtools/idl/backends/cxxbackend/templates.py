from collections import defaultdict

out = defaultdict(dict)

out['AdapterHeader']['head'] = """#ifndef @ig@
#define @ig@
#include <map>
#include <dbus/dbus.h>
#include <dbus_ptr.h>
#include <@suffix@.h>
#include <@interface@.hpp>
"""

out['AdapterHeader']['interface'] = """\
class @class_name@ : public virtual @inherits@
{
public:
  @class_name@(dboost::server& s);
  virtual ~@class_name@();

  virtual DBusHandlerResult handle_message(DBusConnection* connection, DBusMessage* message) override;

  void add_object(dboost_test::@ancestor@* t, const std::string& name);
  void remove_object(const std::string& name);

private:
  dboost::server& m_server;
  std::map<std::string, dboost_test::database*> m_objects;

  static const char* INTERFACE_NAME;
"""

out['AdapterHeader']['operation'] = """dboost::dbus_ptr<DBusMessage> call_@operation@(dboost_test::@interface@* t, DBusMessage* m);"""

out['AdapterSource']['head'] = """\
#include <map>
#include <dbus/dbus.h>
#include <serializer.h>
#include "@class_name@.h"
#include "server.h"
#include "exception.h"
"""

out['AdapterSource']['module_top'] = """\
namespace @id@
{
const char* timer_adaptor::INTERFACE_NAME = "org.dboost.@interface@";

@class_name@::@class_name@(dboost::server& s)
    : m_server(s)
{
    m_server.register_@suffix@(this, @class_name@::INTERFACE_NAME);
}

@class_name@::~@class_name@()
{
    m_server.unregister_@suffix@(this, @class_name@::INTERFACE_NAME);
}

void @class_name@::add_object(dboost_test::@interface@* t, const std::string& name)
{
    clog << __FUNCTION__ << endl;
    m_server.register_object(name);
    m_objects[name] = t;
}

void @class_name@::remove_object(const std::string& name)
{
    clog << __FUNCTION__ << endl;
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
    clog << __FUNCTION__ << endl;
    assert(dbus_message_has_interface(message, INTERFACE_NAME));
    auto dst = dbus_message_get_path(message);
    if (dst == nullptr || strlen(dst) == 0) {
        cerr << "Unknown message destination" << endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    auto obj = m_objects.find(dst);
    if (obj == m_objects.end()) {
        cerr << "Destination object " << dst << " not found" << endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    auto name = dbus_message_get_member(message);
    if (name == nullptr || strlen(name) == 0) {
        cerr << "Unknown method name " << endl;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    typedef dboost::dbus_ptr<DBusMessage> (@class_name@::*caller)(@interface@*, DBusMessage*);
    typedef map<string, caller> caller_table;
    static const caller_table vtbl {
        @operations@
    };

    auto func = vtbl.find(name);
    if (func == vtbl.end()) {
        cerr << "Wrong method name " << name << endl;
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    auto result = (this->*func->second)(obj->second, message);

    clog << "Reply result" << endl;
    dbus_connection_send(connection, result.get(), 0);
    return DBUS_HANDLER_RESULT_HANDLED;
}

}
"""

out['AdapterSource']['operation'] = """\
dboost::dbus_ptr<DBusMessage> database_adaptor::call_@operation@(dboost_test::database* t, DBusMessage* m)
{
    clog << __FUNCTION__ << endl;
    assert(t && m);

    @params_def@
    @params_serialize@
    @call@
    dboost::dbus_ptr<DBusMessage> result(DBOOST_CHECK(dbus_message_new_method_return(m)));
    @params_out_serialize@
    return result;
"""

out['ProxyHeader']['head'] = """\
#ifndef @ig@
#define @ig@
#include <string>
#include <dbus_ptr.h>
#include <@interface@.hpp>
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
  timer_proxy(dboost::dbus_ptr<DBusConnection> conn, const std::string& bus_name, const std::string& obj_name);
"""

out['ProxyHeader']['operation'] = """virtual @rtype@ @id@(@params@)@raises@ override;"""

out['Serializer']['head'] = """\
#ifndef @ig@
#define @ig@

#include <@interface@.hpp>
"""

out['Serializer']['struct'] = """\
template <typename A>
A& serialize(A& a, @struct@& s)
{
    return a @members@;
}
"""