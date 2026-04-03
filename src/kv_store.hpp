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
    load();
   }

    void put(const std:: string &key,const std:: string &value){
      std::lock_guard<std::mutex> lock(mutex_);
      data_[key] = value;
    }


    std::optional<std::string> get(const std::string &key){
      std::lock_guard<std::mutex> lock(mutex_);
      auto it = data_.find(key);
      if(it != data_.end()){
        return it->second;
      }
      return std::optional<std::string>();
    }


    bool del(const std::string &key){
      std::lock_guard<std::mutex> lock(mutex_);

      if(data_.erase(key)){
        return true;
      }
      return false;

    }
    void save() const{
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
    }
    void load(){

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

     size_t size() const{
      std::lock_guard<std::mutex> lock(mutex_);
      return data_.size();
    }
    void clear(){
      std::lock_guard<std::mutex> lock(mutex_);
      data_.clear();
    }




};
