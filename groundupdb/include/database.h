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
#ifndef DATABASE_H
#define DATABASE_H

#include "query.h"

#include <string>

namespace groundupdb {

class Store {
public:
  Store() = default;
  virtual ~Store() = default;
};

using Set = std::unique_ptr<std::unordered_set<std::string>>;

class KeyValueStore : public Store {
public:
  KeyValueStore() = default;
  virtual ~KeyValueStore() = default;

  // Key-Value user functions
  virtual void                            setKeyValue(std::string key,std::string value) = 0;
  //virtual void                            setKeyValue(std::string key,std::string value, std::string bucket) = 0;
  virtual std::string                     getKeyValue(std::string key) = 0;
  virtual void                            setKeyValue(std::string key,std::unordered_set<std::string> value) = 0;
  virtual Set getKeyValueSet(std::string key) = 0;

  // Key-value management functions
  virtual void                            loadKeysInto(std::function<void(std::string key,std::string value)> callback) = 0;
  virtual void                            clear() = 0;
};

using QueryResult = std::unique_ptr<IQueryResult>;

class IDatabase
{
public:
  IDatabase() = default;
  virtual ~IDatabase() = default;
  virtual std::string                     getDirectory(void) = 0;

  // Key-Value use cases
  virtual void                            setKeyValue(std::string key,std::string value) = 0;
  virtual void                            setKeyValue(std::string key,std::string value, std::string bucket) = 0;
  virtual std::string                     getKeyValue(std::string key) = 0;
  virtual void                            setKeyValue(std::string key,std::unordered_set<std::string> value) = 0;
  virtual void                            setKeyValue(std::string key,std::unordered_set<std::string> value,std::string bucket) = 0;
  virtual Set getKeyValueSet(std::string key) = 0;

  // Query records functions
  virtual QueryResult query(Query& query) const = 0;
  // TODO replace the below with just the generic polymorphic function
  virtual QueryResult query(BucketQuery& query) const = 0;

  // management functions
  static const std::unique_ptr<IDatabase>       createEmpty(std::string dbname);
  static const std::unique_ptr<IDatabase>       createEmpty(std::string dbname,std::unique_ptr<KeyValueStore>& kvStore);
  static const std::unique_ptr<IDatabase>       load(std::string dbname);
  virtual void                  destroy() = 0;

};

}

#endif // DATABASE_H
