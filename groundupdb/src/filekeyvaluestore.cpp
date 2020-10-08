/*
See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  Adam Fowler licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
#include "extensions/extdatabase.h"
#include "extensions/highwayhash.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string>

namespace groundupdbext {

namespace fs = std::filesystem;

class FileKeyValueStore::Impl {
public:
  Impl(std::string fullpath);
  std::string m_fullpath;
  HighwayHash m_hasher;

private:

};

FileKeyValueStore::Impl::Impl(std::string fullpath)
  : m_fullpath(fullpath), m_hasher()
{
  ;
}






FileKeyValueStore::FileKeyValueStore(std::string fullpath)
  : mImpl(std::make_unique<FileKeyValueStore::Impl>(fullpath))
{
  if (!fs::exists(fullpath)) {
      fs::create_directories(fullpath);
  }
}

FileKeyValueStore::~FileKeyValueStore()
{
  ;
}

// Key-Value use cases
void
FileKeyValueStore::setKeyValue(const HashedValue& key,EncodedValue&& value)
{
  std::ofstream os;
  std::string keyHash(std::to_string(key.hash()));
  os.open(mImpl->m_fullpath + "/" + keyHash + ".kv",
          std::ios::out | std::ios::trunc);
  os << value.hasValue() << std::endl;
  os << value.type() << std::endl;
  os << value.length() << std::endl;
  for (auto& b : value.data()) {
    os << (const char)b;
  }
  os << std::endl;
  os << value.hash() << std::endl;
  os.close();
  // TODO allow multiple keys for this hash (read, modify, write)
  os.open(mImpl->m_fullpath + "/" + keyHash + ".key",
          std::ios::out | std::ios::trunc);
  os << value.type() << std::endl;
  os << keyHash.c_str();
  os.close();
}

EncodedValue
FileKeyValueStore::getKeyValue(const HashedValue& key)
{
  std::string keyHash(std::to_string(key.hash()));
  std::ifstream t(mImpl->m_fullpath + "/" + keyHash + ".kv"); // DANGEROUS
  std::string eol; // eol can be multiple characters

  bool hasValue;
  groundupdb::Type type;
  int tint;
  std::string cd;
  int length;
  int hash = 0;
  Bytes bytes;
  t >> hasValue;
  std::getline(t,eol);
  if (hasValue) {
    // type
    t >> tint;
    type = (groundupdb::Type)tint;
    // length
    t >> length;
    // data
    std::getline(t,cd);
    bytes.reserve(cd.length());
    for (auto& c : cd) {
      bytes.push_back((std::byte)c);
    }
    // hash
    t >> hash;
    std::getline(t,eol);
    //std::cout << "  Reading and creating value with length: " << length << ", hash: " << hash << std::endl;
  } else {
    length = 0;
    type = groundupdb::Type::UNKNOWN;
    hash = 0;
  }
  return EncodedValue(type,bytes,length,hash);
}


void
FileKeyValueStore::setKeyValue(const HashedValue& key,const Set& value) {
  // store in _string_set.kl file elements_num<length,value...>...
  std::ofstream os;
  std::string keyHash(std::to_string(key.hash()));
  std::string fp(mImpl->m_fullpath + "/" + keyHash + ".kv");
  os.open(fp,
          std::ios::out | std::ios::trunc);
  os << value->size() << std::endl;
  for (auto val = value->begin();val != value->end(); val++) {
    //std::cout << "  Writing set value with length: " << val->length() << ", hash: " << val->hash() << std::endl;
    os << val->hasValue() << std::endl;
    os << val->type() << std::endl;
    os << val->length() << std::endl;
    for (auto& b : val->data()) {
      os << (const char)b;
    }
    os << std::endl;
    os << val->hash() << std::endl;
  }
  os.close();
  // TODO allow multiple keys for this hash (read, modify, write)
  os.open(mImpl->m_fullpath + "/" + keyHash + ".key",
          std::ios::out | std::ios::trunc);
  os << "set" << std::endl;
  for (auto& b : key.data()) {
    os << (const char)b;
  }
  os << std::endl;
  os.close();
}

Set
FileKeyValueStore::getKeyValueSet(const HashedValue& key) {
  // get from _string_set.kl file
  std::string keyHash(std::to_string(key.hash()));
  std::string fp(mImpl->m_fullpath + "/" + keyHash + ".kv");
  if (!fs::exists(fp)) {
    //std::cout << "FileKeyValueStore::getKeyValueSet returning empty set" << std::endl;
    return std::make_unique<std::unordered_set<EncodedValue>>(); // TODO verify we don't need to specify type here
  }
  std::ifstream t(fp);
  std::unordered_set<EncodedValue> values;

  // read size first
  int entries;
  std::string eol; // eol can be multiple characters
  t >> entries;
  std::getline(t,eol);

  // Each value information
  bool hasValue;
  groundupdb::Type type;
  int tint;
  std::string cd;
  Bytes bytes;
  int length = 0;
  std::size_t hash = 0;
  std::size_t lengthSize = sizeof(length);
  std::size_t hashSize = sizeof(hash);
  for (int i = 0;i < entries;i++) {
    // read hasValue
    t >> hasValue;
    std::getline(t,eol);
    if (hasValue) {
      //std::cout << "  Entry has value" << std::endl;
      // type
      cd.clear();
      std::getline(t,cd);
      //std::cout << "    Type string: " << cd << std::endl;
      //t >> tint; // DOESN'T WORK!
      tint = std::stoi(cd);
      type = (groundupdb::Type)tint;
      // length
      cd.clear();
      std::getline(t,cd);
      //std::cout << "    Length string: " << cd << std::endl;
      //t >> length; // DOESN'T WORK!
      length = std::stoi(cd,&lengthSize);
      // data
      cd.clear();
      std::getline(t,cd);
      bytes.clear();
      bytes.reserve(cd.length());
      //std::cout << "    Expecting " << length << " bytes, read line of " << cd.length() << " bytes" << std::endl;
      //std::cout << "    Reading bytes: ";
      for (auto& c : cd) {
        //std::cout << ".";
        bytes.push_back((std::byte)c);
      }
      //std::cout << std::endl;
      // hash
      cd.clear();
      std::getline(t,cd);
      //std::cout << "    Read hash value (string) of: " << cd << std::endl;
      //t >> hash; // DOESN'T WORK!
      hash = std::stoul(cd,&hashSize);
      //std::cout << "    Reading and creating SET value with length: " << length << ", hash: " << hash << std::endl;
      cd.clear();
    } else {
      //std::cout << "  Entry DOES NOT have value" << std::endl;
      length = 0;
      type = groundupdb::Type::UNKNOWN;
      hash = 0;
    }
    values.emplace(type,bytes,length,hash);
  }
  //std::cout << "FileKeyValueStore::getKeyValueSet returning read results of size: " << values.size() << ", but entries in file of: " << entries << std::endl;
  return std::make_unique<std::unordered_set<EncodedValue>>(values);
}

void
FileKeyValueStore::loadKeysInto(
    std::function<void(const HashedValue& key,EncodedValue value)> callback)
{
  std::string type;
  std::string data;
  std::string eol; // eol can be multiple characters
  std::size_t hash;
  // load any files with .kv in their name
  fs::path fp(mImpl->m_fullpath);
  for (auto& p : fs::directory_iterator(fp)) {
    if (p.exists() && p.is_regular_file()) {
      // check if extension is .kv
      if(".key" == p.path().extension()) {
        // If so, open file

        std::string hashWithExtension = p.path().filename().string();
        // ASSUMPTION always ends with .kv
        std::string strhash = hashWithExtension.substr(0,hashWithExtension.length() - 4); // BUG0000001
        // TODO check that file name is long enough
        // TODO check that file exists before opening

        std::ifstream t(p.path());
        t >> type;
        t >> data;
        // Construct hashed key
        std::stringstream sstream(strhash);
        sstream >> hash;
        char* cdata = new char[data.length()];
        const char* cstr = data.c_str();
        for (unsigned long i = 0;i < data.length();i++) {
          cdata[i] = cstr[i];
        }
        HashedKey key(std::string(cdata,data.length()));
        if ("set" == type) {
          // TODO support set as an encoded value
          //callback(key,getKeyValueSet(key));
        } else {
          callback(key,getKeyValue(key));
        }
      }
    }
  }
}


void
FileKeyValueStore::clear()
{
  if (fs::exists(mImpl->m_fullpath)) {
      fs::remove_all(mImpl->m_fullpath);
  }
}

}
