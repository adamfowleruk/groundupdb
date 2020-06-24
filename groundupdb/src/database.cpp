#include "database.h"
#include "extensions/extdatabase.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

using namespace groundupdb;
using namespace groundupdbext;

// 'Hidden' Database::Impl class here
class EmbeddedDatabase::Impl : public IDatabase {
public:
  Impl(std::string dbname, std::string fullpath);
  ~Impl();

  std::string                 getDirectory(void);

  // Key-Value use cases
  void                        setKeyValue(std::string key,std::string value);
  std::string                 getKeyValue(std::string key);

  // management functions
  static  const std::unique_ptr<IDatabase>    createEmpty(std::string dbname);
  static  const std::unique_ptr<IDatabase>    load(std::string dbname);
  void                        destroy();

private:
  std::string m_name;
  std::string m_fullpath;
  std::unordered_map<std::string,std::string>  m_keyValueStore;
};

EmbeddedDatabase::Impl::Impl(std::string dbname, std::string fullpath)
  : m_name(dbname), m_fullpath(fullpath)
{
  // load any files with .kv in their name
  for (auto& p : fs::directory_iterator(getDirectory())) {
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

        m_keyValueStore.insert({key,value});
      }
    }
  }
}

EmbeddedDatabase::Impl::~Impl() {
  ;
  // Z. [Optional] Flush the latest known state to disc here
}

// Management functions

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::createEmpty(std::string dbname) {
  std::string basedir(".groundupdb");
  if (!fs::exists(basedir)) {
      fs::create_directory(basedir);
  }
  std::string dbfolder(basedir + "/" + dbname);
  if (!fs::exists(dbfolder)) {
      fs::create_directory(dbfolder);
  }
  return std::make_unique<EmbeddedDatabase::Impl>(dbname,dbfolder);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::load(std::string dbname) {
  std::string basedir(".groundupdb");
  std::string dbfolder(basedir + "/" + dbname);
  return std::make_unique<EmbeddedDatabase::Impl>(dbname,dbfolder);
}

void EmbeddedDatabase::Impl::destroy() {
  if (fs::exists(m_fullpath)) {
      fs::remove_all(m_fullpath);
  }
  m_keyValueStore.clear();
}

// Instance users functions

std::string EmbeddedDatabase::Impl::getDirectory() {
  return m_fullpath;
}

void EmbeddedDatabase::Impl::setKeyValue(std::string key,std::string value) {
  std::ofstream os;
  os.open(m_fullpath + "/" + key + "_string.kv", std::ios::out | std::ios::trunc);
  os << value;
  os.close();
  // Also write to our in-memory unordered map
  m_keyValueStore.insert({key,value});

  // QUESTION: What is the above storage mechanism paradigm?
  //   ANSWER: Strongly Consistent (we may lose some data due to fsync delay
  //           if our app crashes / machine powers off)
  // QUESTION: What would it be called if we didn't flush to disc here?
  //   ANSWER: Eventually consistent (E.g. if we flushed data every minute)
}

std::string EmbeddedDatabase::Impl::getKeyValue(std::string key) {
  // Only ever read from our in memory map!
  const auto& v = m_keyValueStore.find(key);
  if (v == m_keyValueStore.end()) {
    return ""; // DANGEROUS! Should be 'not found'. TODO error handling.
  }
  return v->second;

  /*
  std::ifstream t(m_fullpath + "/" + key + "_string.kv");
  std::string value;
  //t >> value;
  //t.close();

  t.seekg(0, std::ios::end);
  value.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  value.assign((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());

  return value;
  */
}



// Thoughts for the future: How efficient in memory is our implementation?
// QUESTION: If we doubled the number of keys, how big would our memory footprint be?
//   ANSWER: Double (assuming key name and values of same complexity)

//      NOW: ASSUME our values are 9 times the size of our keys...
//           i.e. not taking in to account hash sizes (which are consistent)...
//           our key storage in memory is 10% of our variable storage
//           and our value storage is 90% of our variable storage
//           E.g. if we had 1 million keys:-
//             Hash storage was 4MB
//             Key storage was 20MB
//             Value storage was 180MB
//           Our total memory use would be 204MB for 180MB of data

// QUESTION: What if we instead doubled the size of our keys? (I.e. 'mykey' became 'mykeymykey')
//   ANSWER: Memory use would increase by 20MB (~10%)
// QUESTION: What if we instead doubled the size of our values?
//   ANSWER: Memory use would increase by 180MB (~90%)

// QUESTION: What options do we have to improve this?
//   ANSWER: Store raw key names and values on disc, not memory, and lazily load them
//           through indexing by hashes only (i.e. in memory we store key hash -> value hash)
// QUESTION: What are the drawbacks?
//   ANSWER: Hash collisions!


// High level Database client API implementation below




// Embedded Database
EmbeddedDatabase::EmbeddedDatabase(std::string dbname,std::string fullpath)
  : mImpl(std::make_unique<EmbeddedDatabase::Impl>(dbname,fullpath))
{
  ;
}

EmbeddedDatabase::~EmbeddedDatabase() {
  ;
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::createEmpty(std::string dbname) {
  return EmbeddedDatabase::Impl::createEmpty(dbname);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::load(std::string dbname) {
  return EmbeddedDatabase::Impl::load(dbname);
}

void EmbeddedDatabase::destroy() {
  mImpl->destroy();
}

// Instance users functions

std::string EmbeddedDatabase::getDirectory() {
  return mImpl->getDirectory();
}

void EmbeddedDatabase::setKeyValue(std::string key,std::string value) {
  mImpl->setKeyValue(key,value);
}

std::string EmbeddedDatabase::getKeyValue(std::string key) {
  return mImpl->getKeyValue(key);
}


