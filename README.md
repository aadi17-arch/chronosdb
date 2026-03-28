# ChronosDB

> A persistent key-value store in C++ that survives crashes.

## 🚀 Features

- **Custom SkipList Storage Engine:** $O(\log n)$ search and insertion.
- **Thread-safe operations** with `std::mutex`.
- **Binary serialization** for data persistence.
- **Durability with Write-Ahead Logging (WAL).**
- **Automatic Log Compaction (Snapshotting)** to keep the system lean.
- Comprehensive performance and real-world test suites.

## 📦 Build & Test

```bash
# Compile and run the core engine tests
g++ -std=c++17 -o test_kv tests/test_kv.cpp -I src
./test_kv

# Run the Week 2 Real-World Durability Test
g++ -std=c++17 -o test_week2 tests/test-week-2.cpp -I src
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

✅ **Week 3: Advanced Storage Engine COMPLETED**
- Replaced `std::unordered_map` with a hand-crafted **SkipList**.
- Implemented **Staircase Search** and **Random Level Generation**.
- Integrated **WAL** and **Thread Safety** into the custom engine.
- Verified recovery and persistence with the new SkipList data structure.
