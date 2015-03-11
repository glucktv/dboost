from collections import defaultdict

out = defaultdict(dict)

out['adapter']['interface'] = """\
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

out['adapter']['operation'] = """dboost::dbus_ptr<DBusMessage> call_@operation@(dboost_test::@interface@* t, DBusMessage* m);"""

out['proxy']['interface'] = """\
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

out['proxy']['operation'] = """virtual @rtype@ @id@(@params@)@raises@ override;"""