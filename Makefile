########################################
# Makefile for station_uptime and tests
########################################

# Compiler and flags
CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

# Default 'all' target builds both executables
all: station_uptime test_main

# Build the main program
station_uptime: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build the test program
test_main: test_main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Object file dependencies
# If you have a separate .cpp for functions, list it here as well.
main.o: main.cpp functions.h
test_main.o: test_main.cpp functions.h

# Generic rule to compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# 'clean' target removes generated files
clean:
	rm -f station_uptime test_main *.o

# Optional test target to run the unit tests
test: test_main
	./test_main

.PHONY: all clean test
