LDFLAGS  += -lboost_system -pthread -ldbus-1
CPPFLAGS += -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -std=c++11 -I../../test/01-basic-scenario

TARGET = server
SOURCES += $(wildcard *.cpp)

$(TARGET): $(SOURCES) 
	g++ $^ -o $@ $(LDFLAGS) $(CPPFLAGS)

.PHONY: clean

clean:
	rm *.o $(TARGET)
