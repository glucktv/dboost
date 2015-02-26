#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dbus-1.0/dbus/dbus.h>


int main() {
  DBusError err;
  DBusConnection* conn;
  int ret;
  char* sigvalue = "Test";
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
  
  dbus_uint32_t serial = 0; // unique number to associate replies with requests
  DBusMessage* msg;
  DBusMessageIter args;
  
  // register our name on the bus, and check for errors
#if 0
  ret = dbus_bus_request_name(conn, "test.signal.source", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
  if (dbus_error_is_set(&err)) {
    fprintf(stderr, "Name Error (%s)\n", err.message);
    dbus_error_free(&err);
  }
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
    fprintf(stderr, "DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret\n");
    exit(1);
  }
#endif

  // create a signal and check for errors 
  msg = dbus_message_new_method_call("org.dboost.basicscenario",
	"/org/dboost/timer/0", // object name of the signal
	"org.dboost.timer", // interface name of the signal
	"add_timer"); // name of the signal
  if (NULL == msg) 
  { 
    fprintf(stderr, "Message Null\n"); 
  }

  dbus_message_iter_init_append(msg, &args);
  // append arguments onto signal
  long l = 100500;
  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT64, &l)) {
    fprintf(stderr, "Out Of Memory!\n"); 
  }

  // send the message and flush the connection
  if (!dbus_connection_send_with_reply_and_block(conn, msg, 5000, &err)) {
    fprintf(stderr, "Out Of Memory!\n"); 
  }
  dbus_connection_flush(conn);
  
  // free the message 
  dbus_message_unref(msg);
  fprintf(stderr, "done.\n");
  return 0;
}
