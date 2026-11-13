CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude
SRC      = $(wildcard src/*.cpp)
LIBSRC   = $(filter-out src/main.cpp,$(SRC))
TARGET   = scheduler
TESTBIN  = run_tests

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Birim testleri: make test
$(TESTBIN): $(LIBSRC) tests/test_scheduler.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TESTBIN)
	./$(TESTBIN)

# Hizli demo: ornek veriyle hepsini calistir, sonuclari output/ klasorune yaz
demo: $(TARGET)
	./$(TARGET) data/sample.txt --all --export

# Grafikler (python3, pandas, matplotlib gerekir): make viz
viz:
	python3 cpu_project/visualizer.py

clean:
	rm -f $(TARGET) $(TESTBIN) $(TARGET).exe $(TESTBIN).exe

.PHONY: all test demo viz clean
