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
#include <fstream>
#include <filesystem>

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
FileKeyValueStore::setKeyValue(std::string key,std::string value)
{
  std::ofstream os;
  std::string keyHash(std::to_string(mImpl->m_hasher(key)));
  os.open(mImpl->m_fullpath + "/" + keyHash + ".kv",
          std::ios::out | std::ios::trunc);
  os << value;
  os.close();
  // TODO allow multiple keys for this hash (read, modify, write)
  os.open(mImpl->m_fullpath + "/" + keyHash + ".key",
          std::ios::out | std::ios::trunc);
  os << "string" << std::endl;
  os << key;
  os.close();
}

std::string
FileKeyValueStore::getKeyValue(std::string key)
{
  std::ifstream t(mImpl->m_fullpath + "/" + std::to_string(mImpl->m_hasher(key)) + ".kv"); // DANGEROUS
  std::string value;

  t.seekg(0, std::ios::end);
  value.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  value.assign((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());

  return value;
}


void
FileKeyValueStore::setKeyValue(std::string key,std::unordered_set<std::string> value) {
  // store in _string_set.kl file elements_num<length,value...>...
  std::ofstream os;
  std::string keyHash(std::to_string(mImpl->m_hasher(key)));
  std::string fp(mImpl->m_fullpath + "/" + keyHash + ".kv");
  os.open(fp,
          std::ios::out | std::ios::trunc);
  os << value.size() << std::endl;
  for (auto& val: value) {
    os << val.length() << std::endl;
    os << val.c_str() << std::endl;
  }
  os.close();
  // TODO allow multiple keys for this hash (read, modify, write)
  os.open(mImpl->m_fullpath + "/" + keyHash + ".key",
          std::ios::out | std::ios::trunc);
  os << "string_set" << std::endl;
  os << key;
  os.close();
}

std::unique_ptr<std::unordered_set<std::string>>
FileKeyValueStore::getKeyValueSet(std::string key) {
  // get from _string_set.kl file
  std::string fp(mImpl->m_fullpath + "/" + std::to_string(mImpl->m_hasher(key)) + ".kv");
  if (!fs::exists(fp)) {
    return std::make_unique<std::unordered_set<std::string>>();
  }
  std::ifstream t(fp);
  std::unordered_set<std::string> values;
  std::string value;

  // read size first
  int entries;
  std::string eol; // eol can be multiple characters
  t >> entries;
  std::getline(t,eol);
  for (int i = 0;i < entries;i++) {
    // read length
    int sl;
    t >> sl;
    std::getline(t,eol);
    std::getline(t,value);
    values.insert(value);
  }
  return std::make_unique<std::unordered_set<std::string>>(values);
}

void
FileKeyValueStore::loadKeysInto(
    std::function<void(std::string key,std::string value)> callback)
{
  // load any files with .kv in their name
  fs::path fp(mImpl->m_fullpath);
  for (auto& p : fs::directory_iterator(fp)) {
    if (p.exists() && p.is_regular_file()) {
      // check if extension is .kv
      if(".key" == p.path().extension()) {
        // If so, open file

        std::string hashWithExtension = p.path().filename();
        // ASSUMPTION always ends with .kv
        std::string hash = hashWithExtension.substr(0,hashWithExtension.length() - 4); // BUG0000001

        std::ifstream t(p.path());
        std::string key;
        std::string type;
        std::getline(t,type); // string or string_set right now

        t.seekg(0, std::ios::end);
        key.reserve(t.tellg()); // TODO - (type.length() + 1)
        t.seekg(type.length() + 1, std::ios::beg);

        key.assign((std::istreambuf_iterator<char>(t)),
                      std::istreambuf_iterator<char>());


        std::ifstream t2(fp / (hash + ".kv")); // BUG-0000001 improper path/filename
        std::string value;

        t2.seekg(0, std::ios::end);
        value.reserve(t2.tellg());
        t2.seekg(0, std::ios::beg);

        value.assign((std::istreambuf_iterator<char>(t2)),
                      std::istreambuf_iterator<char>());

        callback(key,value);
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
