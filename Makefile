CXX=g++
CXXFLAGS=-std=c++17 -Wall -Iinclude

SRC=src/main.cpp

all: main

main: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o main

TESTS=tests/test_tiles tests/test_yaku tests/test_compound tests/test_search tests/test_print tests/test_limit

test: $(TESTS)
	./tests/test_tiles
	./tests/test_yaku
	./tests/test_compound
	./tests/test_search
	./tests/test_print
	./tests/test_limit

tests/test_tiles: tests/test_tiles.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

tests/test_yaku: tests/test_yaku.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

tests/test_compound: tests/test_compound.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

tests/test_search: tests/test_search.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

tests/test_print: tests/test_print.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

tests/test_limit: tests/test_limit.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f main $(TESTS)

.PHONY: all test clean
