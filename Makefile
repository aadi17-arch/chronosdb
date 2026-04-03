CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = chronosdb

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) src/main.cpp

run: all
	./$(TARGET)

test:
	$(CXX) $(CXXFLAGS) -o test_runner tests/test_kv.cpp
	./test_runner

test-week-2:
	$(CXX) $(CXXFLAGS) -o test_runner tests/test-week-2.cpp
	./test_runner

clean:
	rm -f $(TARGET) test_runner *.db

.PHONY: all run test clean
