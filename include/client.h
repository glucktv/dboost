#ifndef DBOOST_CLIENT_H
#define DBOOST_CLIENT_H

namespace dboost
{

class client
{
public:
  client(DBusBusType bus_type, const std::string& peer_name);
  ~client();

  dbus_connection* connection();
  const char* peer_name();

private:
  dbus_connection* m_connection;
  const std::string m_peer_name;
};

dbus_connection* connection()
{
  return m_connection;
}

const char* peer_name()
{
  return m_peer_name.c_str();
}

};


#endif // DBOOST_CLIENT_H