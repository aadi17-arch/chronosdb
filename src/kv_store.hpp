#pragma once

#include <string>
#include <stdexcept>
#include <mutex>
#include <optional>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

class KVStore {
private:
struct SkipNode{
  std::string key;
  std::string value;
  std::vector<SkipNode*> tower_of_pointers;
      SkipNode(const std::string& key, const std::string& value, int lvl){
      tower_of_pointers.resize(lvl+1,nullptr);
      this->key = key;
      this->value = value;
    }
};
    const int MAX_LEVEL = 16; // assumption it can be more as per data..
    const float p = 0.5f; // coin flip for level decision
    int current_level;
    SkipNode* sentinel; // points -> start of skip list basically a dummy node
    std::string filepath_;
    mutable std::mutex mutex_;
    std::string wal_filepath_;
    mutable std::ofstream wal_out;
    // for random level generation
    std::mt19937 random_number_generator;
    std::uniform_real_distribution<float> random_distribution;

    enum class WalOp : uint8_t { PUT = 0, DEL = 1 };

    void write_string(std::ofstream& out, const std::string& s) const {
        uint32_t len = s.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(s.data(), len);
    }

    std::string read_string(std::ifstream& in) const {
        uint32_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string s(len, '\0');
        in.read(s.data(), len);
        return s;
    }

    void append_to_wal(WalOp op, const std::string& key, const std::string& value = "") {
        if (!wal_out.is_open()) return;
        uint8_t op_code = static_cast<uint8_t>(op);
        wal_out.write(reinterpret_cast<const char*>(&op_code), sizeof(op_code));
        write_string(wal_out, key);
        if (op == WalOp::PUT) {
            write_string(wal_out, value);
        }
        wal_out.flush();
    }

    void recover_from_wal() {
        std::ifstream in(wal_filepath_, std::ios::binary);
        if (!in.is_open()) return;

        while (in.peek() != EOF) {
            uint8_t op_code_raw;
            in.read(reinterpret_cast<char*>(&op_code_raw), sizeof(op_code_raw));
            WalOp op = static_cast<WalOp>(op_code_raw);

            std::string key = read_string(in);
            if (op == WalOp::PUT) {
                std::string value = read_string(in);
                put(key, value);
            } else if (op == WalOp::DEL) {
                del(key);
            }
        }
    }

    //random level generator
    int random_level_generator(){
      int lvl = 0;
      while(random_distribution(random_number_generator) < p && lvl < MAX_LEVEL-1) lvl++;
      return lvl;
    }

    // search through staircase method..
    SkipNode* search(const std::string &key){
      SkipNode* current_node = sentinel;
      for(int i = current_level; i >= 0; --i){
        while( current_node->tower_of_pointers[i] !=nullptr &&  current_node->tower_of_pointers[i]->key < key){
          current_node = current_node->tower_of_pointers[i];
        }
      }
      return current_node->tower_of_pointers[0];
    }

public:
    explicit KVStore(const std::string& filepath) {
        filepath_ = filepath;
        wal_filepath_ = filepath_ + ".log";
        current_level = 0;
        sentinel = new SkipNode("","",MAX_LEVEL);
        random_distribution = std::uniform_real_distribution<float>(0.0f,1.0f);
        random_number_generator.seed(std::random_device{}());
        load();
        recover_from_wal();
        wal_out.open(wal_filepath_, std::ios::app | std::ios::binary);
    }
    std::vector<std::pair<std::string,std::string>> scan(const std::string &start_key, const std::string &end_key){
      std::lock_guard<std::mutex> lock(mutex_);
      std::vector<std::pair<std::string,std::string>> results;
      SkipNode* curr = search(start_key);

      while(curr && curr->key <= end_key){
        results.emplace_back(curr->key, curr->value);
        curr = curr->tower_of_pointers[0];
      }
      return results;

    }
    // scan->prefix..
    std::vector<std::pair<std::string,std::string>> scan_prefix(const std::string &prefix){
      std::string end_key = prefix;
      if(!end_key.empty()){
        end_key.back()++;
      }
      return scan(prefix,end_key);
    }

    // get we are using optinal to return value..
    std::optional<std::string> get(const std::string &key){
      std::lock_guard<std::mutex> lock(mutex_);
      SkipNode* node = search(key);
      if(node && node->key == key) return node->value;
      return std::nullopt;
    }

    // put method -> hard part
    void put(const std::string &key, const std::string &value){
      std::lock_guard<std::mutex> lock(mutex_);
      append_to_wal(WalOp::PUT, key, value);

      // we are storing every pointer address in update vector..
      std::vector<SkipNode*> update(MAX_LEVEL,nullptr);
      SkipNode* current_node = sentinel;
      for(int i = current_level; i>=0;--i){
        while(current_node->tower_of_pointers[i] != nullptr && current_node->tower_of_pointers[i]->key < key){
          current_node = current_node->tower_of_pointers[i];
        }
        update[i] = current_node;
      }
      SkipNode* target_node = current_node->tower_of_pointers[0];
      if(target_node && target_node->key == key){
        target_node->value = value;
        return;
      }

      int new_node_height = random_level_generator();//tells how much hieghts the node should be
      if(new_node_height > current_level){
        for(int i = current_level+1; i <= new_node_height;i++){
           update[i] = sentinel;
        }
        current_level = new_node_height;
      }

      SkipNode* new_node = new SkipNode(key,value,new_node_height);
      for(int i = 0; i <= new_node_height; i++){
         new_node->tower_of_pointers[i] = update[i]->tower_of_pointers[i];
         update[i]->tower_of_pointers[i] = new_node;
      }
    }

    bool del(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        append_to_wal(WalOp::DEL, key);
        std::vector<SkipNode*> update(MAX_LEVEL,nullptr);
        SkipNode* current_node = sentinel;
        for(int i = current_level; i >= 0; i--){
          while(current_node->tower_of_pointers[i] != nullptr && current_node->tower_of_pointers[i]->key < key){
            current_node = current_node->tower_of_pointers[i];
          }
          update[i] = current_node;
        }
        current_node = current_node->tower_of_pointers[0];
        if(current_node && current_node->key == key){
            for(int i = 0; i <= current_level; i++){
                if(update[i]->tower_of_pointers[i] != current_node) break;
                update[i]->tower_of_pointers[i] = current_node->tower_of_pointers[i];
            }
            delete current_node;
            return true;
        }
        return false;
    }

    void save() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ofstream out(filepath_, std::ios::binary | std::ios::trunc);
        if (!out.is_open()) throw std::runtime_error("Failed to open the file");

        uint32_t count = 0;
        SkipNode* curr = sentinel->tower_of_pointers[0];
        while(curr) {
            count++;
            curr = curr->tower_of_pointers[0];
        }
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));

        curr = sentinel->tower_of_pointers[0];
        while(curr) {
            write_string(out, curr->key);
            write_string(out, curr->value);
            curr = curr->tower_of_pointers[0];
        }

        out.flush();
        out.close();

        if (wal_out.is_open()) {
            wal_out.close();
        }
        std::ofstream truncate_log(wal_filepath_, std::ios::binary | std::ios::trunc);
        truncate_log.close();
        wal_out.open(wal_filepath_, std::ios::binary | std::ios::app);
    }

    void load() {
        std::ifstream in(filepath_, std::ios::binary);
        if (!in.is_open()) return;

        uint32_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        for (uint32_t i = 0; i < len; i++) {
            std::string key = read_string(in);
            std::string val = read_string(in);
            put(key, val);
        }
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t count = 0;
        SkipNode* curr = sentinel->tower_of_pointers[0];
        while(curr) {
            count++;
            curr = curr->tower_of_pointers[0];
        }
        return count;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        SkipNode* curr = sentinel->tower_of_pointers[0];
        while(curr) {
            SkipNode* next = curr->tower_of_pointers[0];
            delete curr;
            curr = next;
        }
        for (int i = 0; i <= MAX_LEVEL; i++) sentinel->tower_of_pointers[i] = nullptr;
        current_level = 0;
    }

    ~KVStore() {
        clear();
        delete sentinel;
    }
};
