CXX=g++
CXXFLAGS=-std=c++17 -Wall -Iinclude

SRC=src/main.cpp

all: main

main: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o main

TESTS=tests/test_tiles tests/test_yaku tests/test_compound

test: $(TESTS)
	./tests/test_tiles
	./tests/test_yaku
	./tests/test_compound

tests/test_tiles: tests/test_tiles.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

tests/test_yaku: tests/test_yaku.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

tests/test_compound: tests/test_compound.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f main $(TESTS)

.PHONY: all test clean
