SYSTEMC ?= /opt/systemc

CC     := /usr/bin/g++
CFLAGS := -O2 -Wall -Wextra -pedantic -std=c++17

IDIR := -I ./ -isystem $(SYSTEMC)/include
LDIR := -L ./ -L $(SYSTEMC)/lib64 -L $(SYSTEMC)/lib \
        -Wl,-rpath,$(SYSTEMC)/lib64 -Wl,-rpath,$(SYSTEMC)/lib
LIBS := -lsystemc -lm


EXE     := main.x
SOURCES := dhdemo.cpp dh_sw.cpp dh_hw.cpp dh_components.cpp
HEADERS := $(wildcard *.h)
OBJECTS := $(SOURCES:%.cpp=%.o)
MAKEFILE_FILE := Makefile

$(EXE): $(OBJECTS) $(MAKEFILE_FILE)
	$(CC) $(CFLAGS) $(LDIR) -o $@ $(OBJECTS) $(LIBS) 2>&1 | c++filt

%.o: %.cpp $(HEADERS) $(MAKEFILE_FILE)
	$(CC) $(CFLAGS) $(IDIR) -c $< -o $@


.PHONY: all, clean

all: clean $(EXE)

clean:
	rm -f -v *~ *.vcd $(OBJECTS) $(EXE) core
