# ChronosDB

> A persistent key-value store in C++ that survives crashes.

## 🚀 Features

- Thread-safe operations with `std::mutex`
- Binary serialization for data persistence
- **Durability with Write-Ahead Logging (WAL)**
- **Automatic Log Compaction (Snapshotting)**
- Comprehensive performance and real-world test suites

## 📦 Build & Test

```bash
# Compile and run the main demo
g++ -std=c++17 -o chronosdb src/main.cpp
./chronosdb

# Run the Week 2 Real-World Durability Test
g++ -std=c++17 -o test_week2 tests/test-week-2.cpp
./test_week2
```

## 📝 Status

✅ **Week 1: Core implementation COMPLETED**
- In-memory storage with `std::unordered_map`
- Basic persistence to `.db` binary files

✅ **Week 2: Durability & WAL COMPLETED**
- Write-Ahead Logging (WAL) for crash protection
- Log Compaction during `save()` to keep system lean
- Real-world durability testing verifying recovery success
