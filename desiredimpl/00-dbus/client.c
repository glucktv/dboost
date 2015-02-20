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
  ret = dbus_bus_request_name(conn, "test.signal.source", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
  if (dbus_error_is_set(&err)) {
    fprintf(stderr, "Name Error (%s)\n", err.message);
    dbus_error_free(&err);
  }
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
    fprintf(stderr, "DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret\n");
    exit(1);
  }
    
  // create a signal and check for errors 
  msg = dbus_message_new_signal("/test/signal/Object", // object name of the signal
	"test.signal.Type", // interface name of the signal
	"Test"); // name of the signal
  if (NULL == msg) 
  { 
    fprintf(stderr, "Message Null\n"); 
  }

  // append arguments onto signal
  dbus_message_iter_init_append(msg, &args);
  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &sigvalue)) { 
    fprintf(stderr, "Out Of Memory!\n"); 
  }

  // send the message and flush the connection
  if (!dbus_connection_send(conn, msg, &serial)) { 
    fprintf(stderr, "Out Of Memory!\n"); 
  }
  dbus_connection_flush(conn);
  
  // free the message 
  dbus_message_unref(msg);
  fprintf(stderr, "done.\n");
  return 0;
}
