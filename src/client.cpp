#include <client.h>

client::client(DBusBusType bus_type, const std::string& peer_name)
{
  DBusError error;
  m_connection = dbus_bus_get(bus_type, &error);
  DBOOST_CHECK_RESULT_WITH_ERR(m_connection, error);
}

client::~client()
{
  dbus_connection_disconnect(m_connection);
}
