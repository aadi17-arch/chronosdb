#include <iostream>
#include <cassert>
#include <filesystem>
#include "../src/kv_store.hpp"

namespace fs = std::filesystem;

int main() {
    std::string db_file = "durability.db";
    std::string log_file = db_file + ".log";

    // Clean start
    if (fs::exists(db_file)) fs::remove(db_file);
    if (fs::exists(log_file)) fs::remove(log_file);

    std::cout << "--- 🛡️ STARTING DURABILITY TEST 🛡️ ---" << std::endl;

    // 1. PHASE 1: Write and Crash
    {
        KVStore db(db_file);
        db.put("status", "I survived!");
        db.put("system", "Online");
        std::cout << "[WORK] Data written to WAL. Crashing now without save()..." << std::endl;
        // Simulating crash: Object goes out of scope without calling save()
    }

    // 2. PHASE 2: Recovery
    {
        std::cout << "[RECOVERY] Re-starting system..." << std::endl;
        KVStore db(db_file);
        
        auto status = db.get("status");
        auto system = db.get("system");

        assert(status.has_value() && status.value() == "I survived!");
        assert(system.has_value() && system.value() == "Online");
        
        std::cout << "✅ Recovery SUCCESS: Data survived from WAL!" << std::endl;
    }

    std::cout << "--- ✨ DURABILITY TEST PASSED! ✨ ---" << std::endl;
    return 0;
}
