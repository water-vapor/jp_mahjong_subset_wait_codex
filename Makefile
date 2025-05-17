CXX=g++
CXXFLAGS=-std=c++17 -Wall -Iinclude

SRC=src/main.cpp

all: main

main: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o main

test: tests/test_tiles

tests/test_tiles: tests/test_tiles.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f main tests/test_tiles

.PHONY: all test clean
