CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = chronosdb

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) src/main.cpp

run: all
	./$(TARGET)

# Runs functional engine tests (KV, Scans, Prefix)
test:
	$(CXX) $(CXXFLAGS) -o test_kv_store tests/test_kv_store.cpp
	./test_kv_store

# Runs durability/crash recovery tests
test-durability:
	$(CXX) $(CXXFLAGS) -o test_durability tests/test_durability.cpp
	./test_durability

clean:
	rm -f $(TARGET) test_kv_store test_durability *.db *.log

.PHONY: all run test clean
