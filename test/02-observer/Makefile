.PHONY: all clean

OMNIIDL := omniidl 
BE_PATH := -p ../../buildtools/idl/backends

%.hpp : %.idl
	$(OMNIIDL) $(BE_PATH) -bcxxbackend -K $^

all : $(patsubst %.idl,%.hpp,$(wildcard *.idl))

clean :
	rm -f *.hpp
