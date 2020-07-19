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
#include "database.h"
#include "query.h"
#include "extensions/extquery.h"
#include "extensions/extdatabase.h"

#include <filesystem>

using namespace groundupdb;
using namespace groundupdbext;

namespace fs = std::filesystem;

// 'Hidden' Database::Impl class here
class EmbeddedDatabase::Impl : public IDatabase {
public:
  Impl(std::string dbname, std::string fullpath);
  Impl(std::string dbname, std::string fullpath,
       std::unique_ptr<KeyValueStore>& kvStore);
  ~Impl();

  std::string                     getDirectory(void);

  // Key-Value use cases
  void                            setKeyValue(std::string key,std::string value);
  void                            setKeyValue(std::string key,std::string value, std::string bucket);
  std::string                     getKeyValue(std::string key);
  void                            setKeyValue(std::string key,std::unordered_set<std::string> value);
  std::unique_ptr<std::unordered_set<std::string>> getKeyValueSet(std::string key);

  // Query functions
  std::unique_ptr<IQueryResult>    query(Query& query) const;
  std::unique_ptr<IQueryResult>    query(BucketQuery& query) const;

  // management functions
  static  const std::unique_ptr<IDatabase>    createEmpty(std::string dbname);
  static  const std::unique_ptr<IDatabase>    createEmpty(std::string dbname,std::unique_ptr<KeyValueStore>& kvStore);
  static  const std::unique_ptr<IDatabase>    load(std::string dbname);
  void                        destroy();

  std::string m_name;
  std::string m_fullpath;
  std::unique_ptr<KeyValueStore> m_keyValueStore;
  std::unique_ptr<KeyValueStore> m_indexStore;
};

EmbeddedDatabase::Impl::Impl(std::string dbname, std::string fullpath)
  : m_name(dbname), m_fullpath(fullpath)
{
  // Explicitly specify base type so it matches the make_unique expected class (KeyValueStore)
  std::unique_ptr<KeyValueStore> fileStore = std::make_unique<FileKeyValueStore>(fullpath);
  std::unique_ptr<KeyValueStore> memoryStore = std::make_unique<MemoryKeyValueStore>(fileStore);
  m_keyValueStore = std::move(memoryStore);

  // Explicitly specify base type so it matches the make_unique expected class (KeyValueStore)
  std::unique_ptr<KeyValueStore> fileIndexStore = std::make_unique<FileKeyValueStore>(fullpath + "/.indexes");
  std::unique_ptr<KeyValueStore> memIndexStore = std::make_unique<MemoryKeyValueStore>(fileIndexStore);
  m_indexStore = std::move(memIndexStore);
}

EmbeddedDatabase::Impl::Impl(std::string dbname, std::string fullpath,
     std::unique_ptr<KeyValueStore>& kvStore)
  : m_name(dbname), m_fullpath(fullpath), m_keyValueStore(kvStore.release())
{
  // Explicitly specify base type so it matches the make_unique expected class (KeyValueStore)
  std::unique_ptr<KeyValueStore> fileIndexStore = std::make_unique<FileKeyValueStore>(fullpath + "/.indexes");
  std::unique_ptr<KeyValueStore> memIndexStore = std::make_unique<MemoryKeyValueStore>(fileIndexStore);
  m_indexStore = std::move(memIndexStore);
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
  return std::make_unique<EmbeddedDatabase::Impl>(dbname,dbfolder);
}


const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::createEmpty(std::string dbname,std::unique_ptr<KeyValueStore>& kvStore) {
  std::string basedir(".groundupdb");
  if (!fs::exists(basedir)) {
      fs::create_directory(basedir);
  }
  std::string dbfolder(basedir + "/" + dbname);
  return std::make_unique<EmbeddedDatabase::Impl>(dbname,dbfolder,kvStore);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::load(std::string dbname) {
  std::string basedir(".groundupdb");
  std::string dbfolder(basedir + "/" + dbname);
  return std::make_unique<EmbeddedDatabase::Impl>(dbname,dbfolder);
}

void EmbeddedDatabase::Impl::destroy() {
  m_keyValueStore->clear();
}

// Instance users functions

std::string EmbeddedDatabase::Impl::getDirectory() {
  return m_fullpath;
}

void EmbeddedDatabase::Impl::setKeyValue(std::string key,std::string value) {
  m_keyValueStore->setKeyValue(key,value);


  // QUESTION: What is the above storage mechanism paradigm?
  //   ANSWER: Strongly Consistent (we may lose some data due to fsync delay
  //           if our app crashes / machine powers off)
  // QUESTION: What would it be called if we didn't flush to disc here?
  //   ANSWER: Eventually consistent (E.g. if we flushed data every minute)
}

std::string EmbeddedDatabase::Impl::getKeyValue(std::string key) {
  return m_keyValueStore->getKeyValue(key);
}

void EmbeddedDatabase::Impl::setKeyValue(std::string key,std::string value, std::string bucket) {
  setKeyValue(key,value);

  // Add to bucket index
  std::string idxKey("bucket::" + bucket);
  // query the key index
  std::unique_ptr<std::unordered_set<std::string>> recordKeys(m_indexStore->getKeyValueSet(idxKey));
  recordKeys.get()->insert(key);
  m_indexStore->setKeyValue(idxKey,*recordKeys.release()); // TODO do we need this? Yes - may not be in memory store
  // TODO replace the above with appendKeyValueSet(key,value)
}

void EmbeddedDatabase::Impl::setKeyValue(std::string key,std::unordered_set<std::string> value) {
  m_keyValueStore->setKeyValue(key,value);
}

std::unique_ptr<std::unordered_set<std::string>> EmbeddedDatabase::Impl::getKeyValueSet(std::string key) {
  return m_keyValueStore->getKeyValueSet(key);
}

// Query functions

std::unique_ptr<IQueryResult>
EmbeddedDatabase::Impl::query(Query& q) const {
  //return std::make_unique<DefaultQueryResult>(); // no results
  // Query is abstract, so try overloading here
  return query(static_cast<decltype(q)>(q));
}


std::unique_ptr<IQueryResult>
EmbeddedDatabase::Impl::query(BucketQuery& query) const {
  // Bucket query
  // construct a name for our key index
  std::string idxKey("bucket::" + query.bucket());
  // query the key index

  std::unique_ptr<IQueryResult> r = std::make_unique<DefaultQueryResult>(m_indexStore->getKeyValueSet(idxKey));
  //std::cout << "EDB::Impl:query result size: " << r.get()->recordKeys()->size() << std::endl;
  return std::move(r);
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


EmbeddedDatabase::EmbeddedDatabase(std::string dbname, std::string fullpath,
                                   std::unique_ptr<KeyValueStore>& kvStore)
  : mImpl(std::make_unique<EmbeddedDatabase::Impl>(dbname,fullpath,kvStore))
{
  ;
}

EmbeddedDatabase::~EmbeddedDatabase() {
  ;
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::createEmpty(std::string dbname) {
  return EmbeddedDatabase::Impl::createEmpty(dbname);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::createEmpty(std::string dbname,std::unique_ptr<KeyValueStore>& kvStore) {
  return EmbeddedDatabase::Impl::createEmpty(dbname, kvStore);
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

void EmbeddedDatabase::setKeyValue(std::string key,std::string value, std::string bucket) {
  mImpl->setKeyValue(key,value,bucket);
}

std::string EmbeddedDatabase::getKeyValue(std::string key) {
  return mImpl->getKeyValue(key);
}

void EmbeddedDatabase::setKeyValue(std::string key,std::unordered_set<std::string> value) {
  mImpl->setKeyValue(key,value);
}

std::unique_ptr<std::unordered_set<std::string>> EmbeddedDatabase::getKeyValueSet(std::string key) {
  return mImpl->getKeyValueSet(key);
}

// MARK: Query functions

std::unique_ptr<IQueryResult>
EmbeddedDatabase::query(Query& query) const {
  return mImpl->query(query);
}
std::unique_ptr<IQueryResult>
EmbeddedDatabase::query(BucketQuery& query) const {
  return mImpl->query(query);
}


