cat > Makefile << 'EOF'
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = chronosdb
SOURCES = src/main.cpp
HEADERS = src/kv_store.hpp

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.db *.o

help:
	@echo "ChronosDB Makefile"
	@echo "  make all   - Build the project"
	@echo "  make run   - Build and run"
	@echo "  make clean - Remove build artifacts"
EOF
