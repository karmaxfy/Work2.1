CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude
SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard include/*.h)

all: 1laba

1laba: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o 1laba

clean:
	rm -f 1laba

.PHONY: all clean
