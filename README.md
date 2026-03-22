# ChronosDB

> A persistent key-value store in C++ that survives crashes.

## 🚀 Features

- Thread-safe operations
- Binary serialization
- File persistence
- Write-ahead logging (planned)

## 📦 Build

```bash
make
make run
```

## 📝 Status

✅ **Week 1: Core implementation COMPLETED**
- In-memory storage with `std::unordered_map`
- Thread safety with `std::mutex`
- Binary serialization for data persistence
- Comprehensive test suite in `tests/`
