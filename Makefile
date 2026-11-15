CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude
SRC      = $(wildcard src/*.cpp)
TARGET   = scheduler

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
