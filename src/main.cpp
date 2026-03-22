#include<iostream>
#include"kv_store.hpp"


int main(){
  KVStore db("chronos.db");
  db.put("name","chronosdb");
  db.put("version","1.0");
  db.put("author","adi");
  auto name = db.get("name");
  if(name.has_value()){
    std::cout<<"name: "<<name.value() <<std::endl;
  }
  auto result = db.get("some_key");
  if(result.has_value()){
    std::cout << "some_key: " << result.value( )<< std::endl;
  }
  else{
    std::cout <<"Uh oh NOT FOUND !"<< std::endl;
  }
  db.del("name");
  auto del_or_not = db.get("name");
  if(!del_or_not.has_value()){
    std::cout << "name is deleted"<<std::endl;
  }
  db.save();
  KVStore db2("chronos.db");

  std::cout<<db2.get("name").value_or("Not Found");

  return  0;

}
