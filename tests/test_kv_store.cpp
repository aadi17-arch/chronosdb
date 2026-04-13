#include "../src/kv_store.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

// --- KV LOGIC TESTS ---

void test_basic_put_get() {
    KVStore db("test1.db");
    db.put("name", "tom");
    db.put("age", "20");
    assert(db.get("name").value() == "tom");
    assert(db.get("age").value() == "20");
    std::cout << "✅ Basic Put/Get: PASSED" << std::endl;
}

void test_persistence() {
    {
        KVStore db("test2.db");
        db.put("language", "C++");
        db.save();
    }
    KVStore db2("test2.db");
    assert(db2.get("language").value() == "C++");
    std::cout << "✅ Disk Persistence: PASSED" << std::endl;
}

// --- SCAN & PREFIX TESTS ---

void test_range_scan() {
    KVStore db("test_scan.db");
    db.clear();
    db.put("charlie", "3");
    db.put("apple", "1");
    db.put("date", "4");
    db.put("banana", "2");

    // Scan b to d
    auto results = db.scan("banana", "date");
    assert(results.size() == 3);
    assert(results[0].first == "banana");
    assert(results[2].first == "date");
    std::cout << "✅ Range Scan: PASSED" << std::endl;
}

void test_prefix_search() {
    KVStore db("test_prefix.db");
    db.clear();
    db.put("user:101", "Alice");
    db.put("user:102", "Bob");
    db.put("admin:01", "Owner");
    db.put("user:103", "Charlie");

    auto users = db.scan_prefix("user:");
    assert(users.size() == 3);
    assert(users[0].first == "user:101");
    assert(users[2].first == "user:103");
    std::cout << "✅ Prefix Search: PASSED" << std::endl;
}

int main() {
    std::cout << "--- 🚀 STARTING ENGINE UNIT TESTS 🚀 ---" << std::endl;
    test_basic_put_get();
    test_persistence();
    test_range_scan();
    test_prefix_search();
    std::cout << "--- ✨ ALL ENGINE TESTS PASSED! ✨ ---" << std::endl;
    return 0;
}
