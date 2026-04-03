#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <filesystem>
#include "../src/kv_store.hpp"

namespace fs = std::filesystem;

int main() {
    std::string db_file = "production.db";
    std::string log_file = "production.db.log";

    // Start fresh: Clean up any old files from previous tests
    if (fs::exists(db_file)) fs::remove(db_file);
    if (fs::exists(log_file)) fs::remove(log_file);

    std::cout << "--- 🏢 STARTING REAL WORLD SESSION 🏢 ---" << std::endl;

    // 1. STAGE 1: The Initial Configuration
    {
        KVStore db(db_file);
        std::cout << "[INIT] Seeding database with 100 entries..." << std::endl;
        for (int i = 0; i < 100; ++i) {
            db.put("user:" + std::to_string(i), "data_" + std::to_string(i));
        }

        std::cout << "[SNAPSHOT] Saving checkpoint 1 (Compacting Log)..." << std::endl;
        db.save();

        // 🛡️ Verify: The log file should now be EMPTY because of compaction!
        if (fs::file_size(log_file) == 0) {
            std::cout << "✅ Log Compaction worked! Log file is empty after save()." << std::endl;
        } else {
            std::cout << "❌ Log Compaction failed! Log file still has data." << std::endl;
        }
    }

    // 2. STAGE 2: The Afternoon Rush (New data & Deletions)
    {
        KVStore db(db_file);

        std::cout << "[WORK] Deleting users 0-10..." << std::endl;
        for (int i = 0; i < 11; ++i) {
            db.del("user:" + std::to_string(i));
        }

        std::cout << "[WORK] Updating users 50-60..." << std::endl;
        for (int i = 50; i <= 60; ++i) {
            db.put("user:" + std::to_string(i), "NEW_DATA_FOR_USER_" + std::to_string(i));
        }

        std::cout << "[WORK] Adding 'admin_status' -> 'ACTIVE'..." << std::endl;
        db.put("admin_status", "ACTIVE");

        std::cout << "--- 🚨 SUDDEN CRASH! 🚨 ---" << std::endl;
        // Crash! We EXIT without calling db.save().
    }

    // 3. STAGE 3: THE RECOVERY (The Moment of Truth)
    {
        std::cout << "[RECOVERY] Re-starting system after the crash..." << std::endl;
        KVStore db(db_file);

        std::cout << "[VERIFY] Checking if data survived..." << std::endl;

        // Check a deleted user (should be GONE)
        assert(!db.get("user:5").has_value());
        std::cout << "✅ User 5 is still gone (DEL survived)." << std::endl;

        // Check an updated user (should be the NEW value)
        assert(db.get("user:55").value() == "NEW_DATA_FOR_USER_55");
        std::cout << "✅ User 55 still has new data (Update survived)." << std::endl;

        // Check the brand new key
        assert(db.get("admin_status").value() == "ACTIVE");
        std::cout << "✅ 'admin_status' survived the crash! (WAL replayed)." << std::endl;

        // Check a master record from checkpoint 1 (should still be there)
        assert(db.get("user:99").value() == "data_99");
        std::cout << "✅ User 99 survived from the original snapshot." << std::endl;
    }

    std::cout << "--- ✨ ALL REAL-WORLD TESTS PASSED! ✨ ---" << std::endl;

    return 0;
}
