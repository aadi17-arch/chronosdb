#include "../src/kv_store.hpp"
#include <cassert>
#include <iostream>

// Test 1: Basic put & get
void test_basic_put_get() {
    KVStore db("test1.db");
    db.put("name", "tom");
    db.put("age", "20");

    assert(db.get("name").value() == "tom");
    assert(db.get("age").value() == "20");

    std::cout << "Test 1: Basic put & get - PASSED" << std::endl;
    std::cout<< db.size() << "\n";
}

// Test 2: Overwrite existing key
void test_overwrite_key() {
    KVStore db("test2.db");
    db.put("color", "red");
    db.put("color", "blue");

    assert(db.get("color").value() == "blue");

    std::cout << "Test 2: Overwrite existing key - PASSED" << std::endl;
}

// Test 3: Get non-existent key
void test_get_nonexistent() {
    KVStore db("test3.db");
    auto result = db.get("fake_key");

    assert(!result.has_value());

    std::cout << "Test 3: Get non-existent key - PASSED" << std::endl;
}

// Test 4: Delete existing key
void test_delete_key() {
    KVStore db("test4.db");
    db.put("city", "delhi");

    assert(db.del("city") == true);
    assert(!db.get("city").has_value());

    std::cout << "Test 4: Delete existing key - PASSED" << std::endl;
}

// Test 5: Delete non-existent key
void test_delete_nonexistent() {
    KVStore db("test5.db");

    assert(db.del("never_existed") == false);

    std::cout << "Test 5: Delete non-existent key - PASSED" << std::endl;
}

// Test 6: Save and reload from disk
void test_persistence() {
    // Save data
    {
        KVStore db("test6.db");
        db.put("language", "C++");
        db.put("project", "ChronosDB");
        db.save();
    }

    // Reload in a new store
    KVStore db2("test6.db");
    assert(db2.get("language").value() == "C++");
    assert(db2.get("project").value() == "ChronosDB");
    assert(db2.size() == 2);

    std::cout << "Test 6: Save and reload - PASSED" << std::endl;
}

// Test 7: Save empty store & reload
void test_empty_store() {
    {
        KVStore db("test7.db");
        db.save();
    }

    KVStore db2("test7.db");
    assert(db2.size() == 0);

    std::cout << "Test 7: Empty store save & reload - PASSED" << std::endl;
}

int main() {
    std::cout << "==============================" << std::endl;
    std::cout << "  ChronosDB Test Suite" << std::endl;
    std::cout << "==============================" << std::endl;

    test_basic_put_get();
    test_overwrite_key();
    test_get_nonexistent();
    test_delete_key();
    test_delete_nonexistent();
    test_persistence();
    test_empty_store();

    std::cout << "==============================" << std::endl;
    std::cout << "  All tests passed!" << std::endl;
    std::cout << "==============================" << std::endl;


    return 0;
}
