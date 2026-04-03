#pragma once

#include<unordered_map>
#include<string>
#include<stdexcept>
#include<mutex>
#include<optional>
#include<fstream>

class KVStore{
  private:

  std::unordered_map<std::string,std::string> data_;
  std::string filepath_; // path to the file where the data is stored
  mutable std::mutex mutex_;
  // log writing variables..
  // wal -> write ahead log
  std::string wal_filepath_; // for write after logging we are creating a log file here..
  mutable std::ofstream wal_out;

  enum class WalOp : uint8_t{PUT = 0, DEL =1}; //put->insert & del->delete..

  // for writing into --> file
   void write_string(std::ofstream & out,const std::string& s) const{
    uint32_t len = s.size();
    out.write(reinterpret_cast<const char*>(&len),sizeof(len));
    out.write(s.data(),len);
   }
   // for reading --> file
   std::string read_string(std::ifstream & in)const{
    uint32_t len = 0;
    in.read(reinterpret_cast<char*>(&len),sizeof(len));
    std::string s(len,'\0');
    in.read(s.data(),len);
    return s;
   }
   public:
   explicit KVStore(const std::string & filepath){
    filepath_ = filepath; //storing parameter in  member variable
    wal_filepath_ = filepath_ + ".log";

    load();
    recover_from_wal();
    wal_out.open(wal_filepath_,std::ios::app|std::ios::binary); // app -> append..

   }

    void put(const std:: string &key,const std:: string &value){ // ins or upd..
      std::lock_guard<std::mutex> lock(mutex_);
      append_to_wal(WalOp::PUT,key,value);
      data_[key] = value;

    }


    std::optional<std::string> get(const std::string &key){ // for checking if key is present or not
      std::lock_guard<std::mutex> lock(mutex_);
      auto it = data_.find(key);
      if(it != data_.end()){
        return it->second;
      }
      return std::optional<std::string>();
    }


    bool del(const std::string &key){
      std::lock_guard<std::mutex> lock(mutex_);
      append_to_wal(WalOp::DEL , key);

      if(data_.erase(key)){
        return true;
      }
      return false;

    }
    void save() const{ // save data -> file
      std::lock_guard<std::mutex> lock(mutex_);
      std::ofstream out(filepath_,std::ios::binary|std::ios::trunc);
      if(!out.is_open()){
        throw std::runtime_error("Failed to open the file");
      }
      uint32_t len = data_.size();
      out.write(reinterpret_cast<const char*>(&len),sizeof(len));
      for(const auto& [key,val] : data_){
        write_string(out,key);
        write_string(out,val);

      }
      out.flush();
      out.close();
      if(wal_out.is_open  ()){
        wal_out.close();
      }
      std::ofstream truncate_log_clear(wal_filepath_,std::ios::binary|std::ios::trunc);
      truncate_log_clear.close();

      wal_out.open(wal_filepath_,std::ios::binary|std::ios::app);
    }
    void load(){ // load from -> file

      std::ifstream in(filepath_,std::ios::binary);
      if(!in.is_open()){
        return ;
      }
      uint32_t len = 0;
      in.read(reinterpret_cast<char*>(&len),sizeof(len));
      for(uint32_t i = 0; i < len; i++){
        std::string key = read_string(in);
        std::string val = read_string(in);
        data_[key] = val;
      }
    }

     size_t size() const{ // how much data is in database..
      std::lock_guard<std::mutex> lock(mutex_);
      return data_.size();
    }
    void clear(){// clear all data
      std::lock_guard<std::mutex> lock(mutex_);
      data_.clear();
    }

     void append_to_wal(WalOp op,const std::string &key,const std::string &value = ""){ // writing a log file
      if(!wal_out.is_open()) return;
      uint8_t op_code = static_cast<uint8_t>(op); // just here trying to get op value like is it del or put
      wal_out.write(reinterpret_cast<const char*>(&op_code),sizeof(op_code)); // writing op code
      write_string(wal_out,key);
      if(op == WalOp::PUT){ //PUT = 0, DEL =1
        write_string(wal_out, value);
      }
      wal_out.flush();
     }
     void recover_from_wal(){
      std::ifstream in(wal_filepath_, std::ios::binary);
      if(!in.is_open()) return;

      while(in.peek() != EOF){
        uint8_t op_code_raw;
        in.read(reinterpret_cast<char*>(&op_code_raw), sizeof(op_code_raw));
        WalOp op = static_cast<WalOp>(op_code_raw);

        std::string key = read_string(in); // reading key
        if(op == WalOp::PUT){
          std::string value = read_string(in); // reading value
          data_[key] = value;
        }
        else if(op == WalOp::DEL){
          data_.erase(key);
        }
      }
     }





};
