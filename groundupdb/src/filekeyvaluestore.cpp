#include "extensions/extdatabase.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace groundupdbext {

namespace fs = std::filesystem;

class FileKeyValueStore::Impl {
public:
  Impl(std::string fullpath);
  std::string m_fullpath;

private:

};

FileKeyValueStore::Impl::Impl(std::string fullpath)
  : m_fullpath(fullpath)
{
  ;
}






FileKeyValueStore::FileKeyValueStore(std::string fullpath)
  : mImpl(std::make_unique<FileKeyValueStore::Impl>(fullpath))
{
  if (!fs::exists(fullpath)) {
      fs::create_directory(fullpath);
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
  os.open(mImpl->m_fullpath + "/" + key + "_string.kv",
          std::ios::out | std::ios::trunc);
  os << value;
  os.close();
}

std::string
FileKeyValueStore::getKeyValue(std::string key)
{
  std::ifstream t(mImpl->m_fullpath + "/" + key + "_string.kv"); // DANGEROUS
  std::string value;

  t.seekg(0, std::ios::end);
  value.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  value.assign((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());

  return value;
}


void
FileKeyValueStore::loadKeysInto(
    std::function<void(std::string key,std::string value)> callback)
{
  // load any files with .kv in their name
  for (auto& p : fs::directory_iterator(mImpl->m_fullpath)) {
    if (p.exists() && p.is_regular_file()) {
      // check if extension is .kv
      if(".kv" == p.path().extension()) {
        // If so, open file

        std::string keyWithString = p.path().filename();
        // ASSUMPTION always ends with _string.kv
        std::string key = keyWithString.substr(0,keyWithString.length() - 10); // DANGEROUS!!!

        std::ifstream t(p.path());
        std::string value;

        t.seekg(0, std::ios::end);
        value.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        value.assign((std::istreambuf_iterator<char>(t)),
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
