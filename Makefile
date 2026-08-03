SYSTEMC := /opt/systemc

CC     := /usr/bin/g++
CFLAGS := -O2 -Wall -Wextra -pedantic -std=c++17

IDIR := -I ./ -isystem $(SYSTEMC)/include
LDIR := -L ./ -L $(SYSTEMC)/lib64 -Wl,-rpath=$(SYSTEMC)/lib64
LIBS := -lsystemc -lm


EXE     := main.x
SOURCES := $(wildcard *.cpp)
HEADERS := $(wildcard *.h)
OBJECTS := $(SOURCES:%.cpp=%.o)

$(EXE): $(OBJECTS) Makefile 
	$(CC) $(CFLAGS) $(LDIR) -o $@ $(OBJECTS) $(LIBS) 2>&1 | c++filt

$(OBJECTS): $(SOURCES) $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(IDIR) -c $(SOURCES)


.PHONY: all, clean

all: clean $(EXE)

clean:
	rm -f -v *~ *.vcd $(OBJECTS) $(EXE) core

