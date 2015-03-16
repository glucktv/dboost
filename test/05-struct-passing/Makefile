.PHONY: all clean

OMNIIDL := omniidl 
BE_PATH := -p ../../buildtools/idl/backends

%.hpp : %.idl
	$(OMNIIDL) $(BE_PATH) -bcxxbackend -K $^

#server: server.cpp database_proxy.cpp
#	g++ -std=c++11 server.cpp database_proxy.cpp -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include/ -I../../include -o server

all : $(patsubst %.idl,%.hpp,$(wildcard *.idl))

clean :
	rm -f *.hpp
