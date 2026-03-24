#include <iostream>
#include <fstream>
#include "kv_store.hpp"

// Utility function to check if a file exists
bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

int main() {
    std::string db_file = "durability.db";
    std::string log_file = db_file + ".log";

    if (!file_exists(log_file)) {
        // ----------------------------------------------------
        // Part 1: THE WRITE (Simulating a crash)
        // ----------------------------------------------------
        std::cout << "[STORY] Creating database for the first time..." << std::endl;
        KVStore db(db_file);

        std::cout << "[WRITE] Adding 'status' -> 'I survived the crash'..." << std::endl;
        db.put("status", "I survived the crash!");

        std::cout << "--- 🚨 CRASHING NOW! 🚨 ---" << std::endl;
        std::cout << "[INFO] Notice: We ARE NOT calling db.save()." << std::endl;
        std::cout << "[INFO] The data is only in memory and the log file." << std::endl;

        // Exiting without calling save() simulates a sudden crash/power loss
        return 0;

    } else {
        // ----------------------------------------------------
        // Part 2: THE RECOVERY (Proving durability)
        // ----------------------------------------------------
        std::cout << "[STORY] Re-starting database after the crash..." << std::endl;

        // When we create this object, it calls: load() then recover_from_wal()
        KVStore db(db_file);

        std::cout << "[CHECK] Looking for 'status' in the database..." << std::endl;
        auto result = db.get("status");

        if (result.has_value()) {
            std::cout << "✅ SUCCESS: Found it! Value = " << result.value() << std::endl;
            std::cout << "✨ The data survived the crash because it was replayed from the WAL! ✨" << std::endl;
        } else {
            std::cout << "❌ FAILURE: The data was lost. Something is wrong with the WAL logic." << std::endl;
        }

        // Clean up for the next test
        std::cout << "\n[CLEAN] Deleting log file so you can test again." << std::endl;
        // Optionally, you can now call db.save() to merge everything into the .db file
        db.save();
        std::remove(log_file.c_str());

        return 0;
    }
}
