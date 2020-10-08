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
#ifndef EXTDATABASE_H
#define EXTDATABASE_H

#include "../database.h"

#include <functional>

namespace groundupdbext {

using namespace groundupdb;


// Ephemeral
class MemoryKeyValueStore : public KeyValueStore {
public:
  MemoryKeyValueStore();
  MemoryKeyValueStore(std::unique_ptr<KeyValueStore>& toCache);
  ~MemoryKeyValueStore();

  // Key-Value user functions
  void                            setKeyValue(const HashedValue& key,EncodedValue&& value);
  EncodedValue                    getKeyValue(const HashedValue& key);
  void                            setKeyValue(const HashedValue& key,const Set& value);
  Set                             getKeyValueSet(const HashedValue& key);

  // Key-value management functions
  void                            loadKeysInto(std::function<void(const HashedValue& key,EncodedValue value)> callback);
  void                            clear();

private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};

class FileKeyValueStore : public KeyValueStore {
public:
  FileKeyValueStore(std::string fullpath);
  ~FileKeyValueStore();

  // Key-Value use cases
  void                            setKeyValue(const HashedValue& key,EncodedValue&& value);
  EncodedValue                    getKeyValue(const HashedValue& key);
  void                            setKeyValue(const HashedValue& key,const Set& value);
  Set                             getKeyValueSet(const HashedValue& key);

  void                            loadKeysInto(std::function<void(const HashedValue& key,EncodedValue value)> callback);
  void                            clear();

private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};

class EmbeddedDatabase : public IDatabase {
public:
  EmbeddedDatabase(std::string dbname, std::string fullpath);
  EmbeddedDatabase(std::string dbname, std::string fullpath,
                   std::unique_ptr<KeyValueStore>& kvStore);
  EmbeddedDatabase(std::string dbname, std::string fullpath,
                   std::unique_ptr<KeyValueStore>& kvStore,
                   std::unique_ptr<KeyValueStore>& idxStore);
  ~EmbeddedDatabase();

  std::string                                 getDirectory(void);

  // Key-Value use cases
  void                                        setKeyValue(const HashedValue& key,EncodedValue&& value);
  void                                        setKeyValue(const HashedValue& key,EncodedValue&& value,const std::string& bucket);
  EncodedValue                                getKeyValue(const HashedValue& key);
  void                                        setKeyValue(const HashedValue& key,const Set& value);
  void                                        setKeyValue(const HashedValue& key,const Set& value,const std::string& bucket);
  Set                                         getKeyValueSet(const HashedValue& key);

  // Query records functions
  std::unique_ptr<IQueryResult>                query(Query& query) const;
  std::unique_ptr<IQueryResult>                query(BucketQuery& query) const = 0;

  // management functions
  static  const std::unique_ptr<IDatabase>    createEmpty(std::string dbname);
  static  const std::unique_ptr<IDatabase>    createEmpty(std::string dbname,std::unique_ptr<KeyValueStore>& kvStore);
  static  const std::unique_ptr<IDatabase>    createEmpty(std::string dbname,std::unique_ptr<KeyValueStore>& kvStore,std::unique_ptr<KeyValueStore>& idxStore);
  static  const std::unique_ptr<IDatabase>    load(std::string dbname);
  void                        destroy();

  class Impl;

private:
  std::unique_ptr<Impl> mImpl;
};

}

#endif // EXTDATABASE_H
