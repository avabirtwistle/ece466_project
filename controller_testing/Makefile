SYSTEMC ?= /opt/systemc

CXX      := /usr/bin/g++
CXXFLAGS := -O2 -Wall -Wextra -pedantic -std=c++17

CPPFLAGS := -I. -isystem $(SYSTEMC)/include

LDFLAGS := -L$(SYSTEMC)/lib64 -L$(SYSTEMC)/lib \
           -Wl,-rpath,$(SYSTEMC)/lib64 \
           -Wl,-rpath,$(SYSTEMC)/lib

LDLIBS := -lsystemc -lm

TARGET  := controller_test.x
SOURCES := controller_tb.cpp dh_controller.cpp
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

controller_tb.o: controller_tb.cpp dh_controller.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

dh_controller.o: dh_controller.cpp dh_controller.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) controller_test.vcd