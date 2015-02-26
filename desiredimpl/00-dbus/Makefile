#LDFLAGS = -lboost_system -pthread -ldbus-1
LDFLAGS = -ldbus-1
CPPFLAGS = -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include/ -I/usr/include/dbus-1.0/ 

CLIENT_TARGET = client 
CLIENT_SOURCES += client.c

SERVER_TARGET = server
SERVER_SOURCES += server.c

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_SOURCES) 
	$(CC) $^ -o $@ $(LDFLAGS) $(CPPFLAGS)

$(CLIENT_TARGET): $(CLIENT_SOURCES) 
	$(CC) $^ -o $@ $(LDFLAGS) $(CPPFLAGS)

.PHONY: clean all

clean:
	rm *.o $(TARGET_TARGET) $(SERVER_TERGET)
