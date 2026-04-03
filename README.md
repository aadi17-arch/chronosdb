# ChronosDB

> A persistent key-value store in C++ that survives crashes and provides lightning-fast range queries.

## 🚀 Features

- **Custom SkipList Storage Engine:** $O(\log n)$ search, insertion, and deletion efficiency.
- **Advanced Range Scans:** Fast sorted data retrieval with `scan(start, end)`.
- **Prefix Support:** Search for key categories using `scan_prefix(prefix)`.
- **Thread-safe operations** with `std::mutex`.
- **Durability with Write-Ahead Logging (WAL).**
- **Binary serialization** for fast snapshots and recovery.
- **Memory Safety:** Full destructor and memory management for dynamic SkipNode towers.

## 📦 Build & Test

```bash
# 1. Run core storage engine tests (Put, Get, Del, Save, Recovery)
g++ -std=c++17 -o test_kv tests/test_kv.cpp -I src
./test_kv

# 2. Run the Week 4 Range Scan & Prefix Search tests
g++ -std=c++17 -o test_scan tests/test_scan.cpp -I src
./test_scan

# 3. Run the Week 2 Real-World Durability/Crash Test
g++ -std=c++17 -o test_week2 tests/test-week-2.cpp -I src
./test_week2
```

## 📝 Project Timeline

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
- Proper **Memory Management** with a SkipNode destructor.

✅ **Week 4: Advanced Scans & Range Queries COMPLETED**
- Implemented **Range Scanning** via Level-0 traversal.
- Built-in **Prefix Search** for Category lookups.
- Verified sorted data retrieval with specialized test suites.
