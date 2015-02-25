.PHONY: all clean

OMNIIDL := omniidl 
BE_PATH := -p ../../buildtools/idl/backends

TARGET  = libserver.a
SOURCES += $(wildcard *.cpp)

#LDFLAGS  += -lboost_system -pthread -ldbus-1
#CPPFLAGS += -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -std=c++11 -I../../test/01-basic-scenario

CPPFLAGS += -std=c++11

all : $(TARGET)

%.hpp : %.idl
	$(OMNIIDL) $(BE_PATH) -bcppbackend -K $^

idl : $(patsubst %.idl,%.hpp,$(wildcard *.idl))
	echo "idl"

obj : idl
	$(CXX) -c $(wildcard *.cpp) $(CPPFLAGS) $(LDFLAGS)

$(TARGET) : obj
	$(AR) r $@ $(wildcard *.o)

clean :
	rm -f *.hpp $(TARGET) *.o
