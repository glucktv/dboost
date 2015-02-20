#include <memory>
#include <dbus/dbus.h>
#include <list>
#include <cassert>
#include <algorithm>
#include "traits.h"
#include <iostream>

using namespace std;

#define TRACE() clog << __FUNCTION__ << endl

namespace dboost
{

template <class T>
class dbus_ptr
{
    T* m_ptr;

public:
    dbus_ptr() noexcept: m_ptr(nullptr) {}
    dbus_ptr(const dbus_ptr* rhs) noexcept :
    	m_ptr(rhs.m_ptr)
    {
    	if (m_ptr)
    	    dbus_traits<T>::ref(m_ptr);
    }
    dbus_ptr(T* ptr) noexcept:
    	m_ptr(ptr)
    {
    	if (m_ptr)
    		dbus_traits<T>::ref(m_ptr);
    }
    ~dbus_ptr() noexcept
    {
    	if (m_ptr)
    		dbus_traits<T>::unref(m_ptr);
    }
    dbus_ptr& operator= (const dbus_ptr& rhs) noexcept
    {
    	reset(rhs.m_ptr);
    	return *this;
    }
    T* get() const noexcept
	{
    	return m_ptr;
	}
    void swap(dbus_ptr& rhs) noexcept
    {
    	std::swap(m_ptr, rhs.m_ptr);
    }
    void reset(T* rhs) noexcept
    {
    	dbus_ptr<T> p(rhs);
    	swap(p);
    }
};

template <class T>
bool operator== (const dbus_ptr<T>& lhs, const dbus_ptr<T>& rhs)
{
    return lhs.get() == rhs.get();
}

template <class T>
dbus_ptr<T> mk_ptr(T* ptr)
{
	return dbus_ptr<T>(ptr);
}

struct exception: public std::runtime_error
{
public:
    exception(const DBusError& e): std::runtime_error(e.message) {}
};

class watch1
{
};

class server
{
	string m_path;
    typedef list<dbus_ptr<DBusConnection>> connection_ptr_list;
    connection_ptr_list m_connections;
    dbus_ptr<DBusServer> m_server;

	static dbus_ptr<DBusServer> create_server(const char* address);

	static void handle_new_connection(DBusServer*, DBusConnection* conn, void* data);
	static void handle_unregister(DBusConnection* conn, void* data);
	static DBusHandlerResult handle_message(DBusConnection* conn, DBusMessage* msg, void* data);

	void handle_new_connection_impl(DBusServer*, DBusConnection* conn);
	void handle_unregister_impl(DBusConnection* conn);
	DBusHandlerResult handle_message_impl(DBusConnection* conn, DBusMessage* msg);

public:
	server(const char* address, const char* path);
	~server();

};

// base for concrete dispatchers: must be inherited virtually
class dispatcher
{
    server& m_server;
};

dbus_ptr<DBusServer> server::create_server(const char* address)
{
    DBusError err;
    dbus_error_init(&err);
    dbus_ptr<DBusServer> s(dbus_server_listen(address, &err));
    if (s.get() == nullptr) {
    	// TODO: err leaks here!
    	throw exception(err);
    }
    return s;
}

void server::handle_new_connection(DBusServer* srv, DBusConnection* conn, void* data)
{
	TRACE();
	server* s = reinterpret_cast<server*>(data);
	assert(s != nullptr);
	s->handle_new_connection_impl(srv, conn);
}

server::server(const char* address, const char* path):
	m_server(create_server(address)),
	m_path(path)
{
	dbus_server_set_new_connection_function(m_server.get(), handle_new_connection, this, nullptr);
}

void server::handle_unregister(DBusConnection* conn, void* data)
{
	TRACE();
	server* s = reinterpret_cast<server*>(data);
	assert(s != nullptr);
	s->handle_unregister_impl(conn);
}

void server::handle_unregister_impl(DBusConnection* conn)
{

	auto iter = find(m_connections.begin(), m_connections.end(), mk_ptr(conn));
	assert(iter != m_connections.end());
	m_connections.erase(iter);
}

DBusHandlerResult server::handle_message(DBusConnection* conn,
		DBusMessage* msg, void* data)
{
	TRACE();
	server* s = reinterpret_cast<server*>(data);
	assert(s != nullptr);
	return s->handle_message_impl(conn, msg);
}

DBusHandlerResult server::handle_message_impl(DBusConnection* conn,
		DBusMessage* msg)
{

}

server::~server()
{
    dbus_server_disconnect(m_server.get());
}

void server::handle_new_connection_impl(DBusServer*, DBusConnection* conn)
{
    m_connections.emplace_back(dbus_ptr<DBusConnection>(conn));
    static DBusObjectPathVTable vtbl;
    vtbl.message_function = handle_message;
    vtbl.unregister_function = handle_unregister;
    dbus_connection_register_object_path(conn, m_path.c_str(), &vtbl, this);
}

} // namespace dboost

int main()
{
	cout << "Creating server" << endl;
	dboost::server s("unix:path=./myserver", "myobject");
	cout << "Working..." << endl;
	char c[2];
	cout << "Press ENTER" << endl;
	cin.getline(c, 1);
	return 0;
}
