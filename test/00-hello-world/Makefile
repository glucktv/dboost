LDFLAGS = -lboost_system -pthread

TARGET = hw
SOURCES += hw.cpp

$(TARGET): $(SOURCES) 
	gcc $^ -o $@ $(LDFLAGS) $(CPPFLAGS)

.PHONY: clean

clean:
	rm *.o $(TARGET)
