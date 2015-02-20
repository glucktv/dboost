#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <time.h>
#include <unistd.h>
#include <dbus-1.0/dbus/dbus.h>
#include <vector>
#include <cassert>
#include <boost/asio.hpp>
#include <functional>
#include <list>
#include <memory>

class server
{
  boost::asio::io_service m_service;
  typedef std::vector<boost::asio::posix::stream_descriptor> stream_descriptor_vector;
  stream_descriptor_vector m_descriptors;
  typedef std::list<std::unique_ptr<DBusConnection>> connection_ptr_list;
  connection_ptr_list m_connections;

  void add_watch_impl(DBusWatch* watch);
  void remove_watch_impl(DBusWatch* watch);
  void watch_toggled_impl(DBusWatch* watch);

  void handle_read(DBusWatch* watch, boost::system::error_code ec);
  void handle_write(DBusWatch* watch, boost::system::error_code ec);

  stream_descriptor_vector::iterator find_descriptor(DBusWatch* watch);
  void resubscribe(DBusWatch* watch);

  void on_message_impl(DBusConnection* conn, DBusMessage* msg);
  void handle_new_connection_impl(DBusServer* server, DBusConnection* conn);

public:
  static dbus_bool_t add_watch(DBusWatch* watch, void* data);
  static void remove_watch(DBusWatch* watch, void* data);
  static void watch_toggled(DBusWatch* watch, void* data);
  static DBusHandlerResult on_message(DBusConnection* conn, DBusMessage* msg, void* data);
  static void handle_new_connection(DBusServer* server, DBusConnection* conn, void* data);

  void run();
};

void server::handle_new_connection(DBusServer* server, DBusConnection* conn, void* data)
{
  std::clog << __FUNCTION__ << std::endl;
  server* self = reinterpret_cast<server*>(data);
  assert(self != nullptr);
  self->handle_new_connection_impl(server, conn);
}

void server::handle_new_connection_impl(DBusServer* server, DBusConnection* conn)
{
  
}

dbus_bool_t server::add_watch(DBusWatch* watch, void* data)
{
  std::clog << __FUNCTION__ << std::endl;
  assert(watch != nullptr);
  server* self = reinterpret_cast<server*>(data);
  assert(self != nullptr);
  self->add_watch_impl(watch);
  return true;
}

void server::add_watch_impl(DBusWatch* watch)
{
  std::clog << __FUNCTION__ << std::endl;
  auto it = find_descriptor(watch);
  if (it == m_descriptors.end()) {
    m_descriptors.emplace_back(m_service, dbus_watch_get_unix_fd(watch));
  }
  resubscribe(watch);
}

void server::remove_watch(DBusWatch* watch, void* data)
{
  std::clog << __FUNCTION__ << std::endl;
  assert(watch != nullptr);
  server* self = reinterpret_cast<server*>(data);
  assert(self != nullptr);
  self->remove_watch_impl(watch);
}

void server::remove_watch_impl(DBusWatch* watch)
{ 
  std::clog << __FUNCTION__ << std::endl;
  auto d = find_descriptor(watch);
  assert(d != m_descriptors.end());
  m_descriptors.erase(d);
}

void server::watch_toggled(DBusWatch* watch, void* data)
{
  std::clog << __FUNCTION__ << std::endl;
  assert(watch != nullptr);
  server* self = reinterpret_cast<server*>(data);
  assert(self != nullptr);
  self->watch_toggled_impl(watch);
}
 
void server::watch_toggled_impl(DBusWatch* watch)
{
  std::clog << __FUNCTION__ << std::endl;
  resubscribe(watch);
}

server::stream_descriptor_vector::iterator server::find_descriptor(DBusWatch* watch)
{
  auto fd = dbus_watch_get_unix_fd(watch);
  return find_if(m_descriptors.begin(), m_descriptors.end(), 
                   [fd](boost::asio::posix::stream_descriptor& d) -> bool 
                        { return fd == d.native_handle(); });
}
  
void server::handle_read(DBusWatch* watch, boost::system::error_code ec)
{
  dbus_watch_handle(watch, ec ? DBUS_WATCH_ERROR : DBUS_WATCH_READABLE);
}

void server::handle_write(DBusWatch* watch, boost::system::error_code ec)
{
  dbus_watch_handle(watch, ec ? DBUS_WATCH_ERROR : DBUS_WATCH_WRITABLE);
}

void server::resubscribe(DBusWatch* watch)
{
  std::clog << __FUNCTION__ << std::endl;
  auto flags = dbus_watch_get_flags(watch);
  auto iter = find_descriptor(watch);
  assert(iter != m_descriptors.end());

  using namespace std::placeholders;
  if (flags & DBUS_WATCH_READABLE) {
    iter->async_read_some(boost::asio::null_buffers(), 
                         std::bind(&server::handle_read, this, watch, _1));
  }
  if (flags & DBUS_WATCH_WRITABLE) {
    iter->async_write_some(boost::asio::null_buffers(),
                          std::bind(&server::handle_write, this, watch, _1));

  }
}

DBusHandlerResult server::on_message(DBusConnection* conn, DBusMessage* msg, void* data)
{
  std::clog << __FUNCTION__ << std::endl;
  server* self = reinterpret_cast<server*>(data);
  assert(self != nullptr);
  self->on_message_impl(conn, msg);
  return DBUS_HANDLER_RESULT_HANDLED;
}

void server::on_message_impl(DBusConnection* conn, DBusMessage* msg)
{
}

void server::run()
{
  m_service.run();
}

int main() {
  DBusError err;
  DBusConnection* conn;
  DBusMessage* msg;
  DBusMessageIter args;
  int ret;
  char* sigvalue;
  // initialise the errors
  dbus_error_init(&err); //TODO
  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err)) { 
    fprintf(stderr, "Connection Error (%s)\n", err.message); 
    dbus_error_free(&err); 
  }
  if (NULL == conn) { 
    fprintf(stderr, "Connection to dbus failed\n"); 
  }

  server s;
  dbus_connection_set_watch_functions(conn,
    &server::add_watch,
    &server::remove_watch, 
    &server::watch_toggled,
    &s,
    NULL);
  std::clog << "Starting server" << std::endl;

  DBusObjectPathVTable vtbl;
  vtbl.message_function = &server::on_message;
  vtbl.unregister_function = NULL; 

  dbus_connection_register_object_path(conn, "/test/signal/Object", &vtbl, &s);

  s.run();
#if 0
  while (1) {
      // non blocking read of the next available message
      dbus_connection_read_write(conn, -1);
      msg = dbus_connection_pop_message(conn);
      printf("Got Si3\n");
      // loop again if we haven't read a message
      if (NULL == msg) { 
         sleep(1);
         continue;
      }
      // check if the message is a signal from the correct interface
      // and with the correct name
      if (dbus_message_is_signal(msg, "test.signal.Type", "Test")) {
         // read the parameters
	  printf("Got Si4\n");
         if (!dbus_message_iter_init(msg, &args))
            fprintf(stderr, "Message has no arguments!\n"); 
         else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) 
            fprintf(stderr, "Argument is not string!\n"); 
         else {
            dbus_message_iter_get_basic(&args, &sigvalue);
            printf("Got Signal with value %s\n", sigvalue);
         }
      }
      else
	printf("Got Si\n");

      // free the message
      dbus_message_unref(msg);
   }
#endif
   
}

