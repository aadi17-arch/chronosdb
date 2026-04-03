#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include "kv_store.hpp"

void test_range_scan() {
    KVStore db("test_scan.db");
    db.clear();

    // 1. Insert out of order
    db.put("charlie", "3");
    db.put("apple", "1");
    db.put("date", "4");
    db.put("banana", "2");
    db.put("elephant", "5");

    // 2. Scan from 'b' to 'd'
    std::cout << "Scanning from 'b' to 'd'..." << std::endl;
    auto results = db.scan("banana", "date");

    // Results should be banana, charlie, date
    assert(results.size() == 3);
    assert(results[0].first == "banana");
    assert(results[1].first == "charlie");
    assert(results[2].first == "date");

    // 3. Scan everything
    std::cout << "Scanning all data..." << std::endl;
    auto all = db.scan("", "zzzz");
    assert(all.size() == 5);

    std::cout << "Range Scan Test: PASSED" << std::endl;
}

int main() {
    test_range_scan();
    return 0;
}
