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

#include <unordered_map>

namespace groundupdbext {

class MemoryKeyValueStore::Impl {
public:
  Impl();
  Impl(std::unique_ptr<KeyValueStore>& toCache);

  std::unordered_map<std::string,std::string,HighwayHash> m_keyValueStore;
  std::unordered_map<std::string,std::unordered_set<std::string>,HighwayHash> m_listStore;
  std::optional<std::unique_ptr<KeyValueStore>> m_cachedStore;

private:

};

MemoryKeyValueStore::Impl::Impl()
  : m_keyValueStore(), m_listStore(), m_cachedStore()
{
  ;
}

MemoryKeyValueStore::Impl::Impl(std::unique_ptr<KeyValueStore>& toCache)
  : m_keyValueStore(), m_listStore(), m_cachedStore(toCache.release())
{
  ;
}






MemoryKeyValueStore::MemoryKeyValueStore()
  : mImpl(std::make_unique<MemoryKeyValueStore::Impl>())
{
  ;
}

MemoryKeyValueStore::MemoryKeyValueStore(std::unique_ptr<KeyValueStore>& toCache)
  : mImpl(std::make_unique<MemoryKeyValueStore::Impl>(toCache))
{
  mImpl->m_cachedStore->get()->loadKeysInto([this](std::string key,std::string value) {
    mImpl->m_keyValueStore.insert({key,value});
  });
}


MemoryKeyValueStore::~MemoryKeyValueStore()
{
  ;
}

// Key-Value use cases
void
MemoryKeyValueStore::setKeyValue(std::string key,std::string value)
{
  // Also write to our in-memory unordered map
  // Note: insert on unordered_map does NOT perform an insert if the key already exists
  mImpl->m_keyValueStore.erase(key);
  mImpl->m_keyValueStore.insert({key,value});
  if (mImpl->m_cachedStore) {
    mImpl->m_cachedStore->get()->setKeyValue(key,value);
  }
}

void
MemoryKeyValueStore::setKeyValue(std::string key,std::string value, std::string bucket)
{
  setKeyValue(key,value);
  if (mImpl->m_cachedStore) {
    mImpl->m_cachedStore->get()->setKeyValue(key,value,bucket);
  }

  // Note: Bucket indexing implemented at DATABASE level, not kv store level
}

std::string
MemoryKeyValueStore::getKeyValue(std::string key)
{
  // Only ever read from our in memory map!
  const auto& v = mImpl->m_keyValueStore.find(key);
  if (v == mImpl->m_keyValueStore.end()) {
    return ""; // DANGEROUS! Should be 'not found'. TODO error handling.
  }
  return v->second;
}


void
MemoryKeyValueStore::setKeyValue(std::string key,std::unordered_set<std::string> value) {
  // Note: insert on unordered_map does NOT perform an insert if the key already exists
  mImpl->m_listStore.erase(key);
  mImpl->m_listStore.insert({key,value});
  if (mImpl->m_cachedStore) {
    mImpl->m_cachedStore->get()->setKeyValue(key,value);
  }
}

std::unique_ptr<std::unordered_set<std::string>>
MemoryKeyValueStore::getKeyValueSet(std::string key) {
  const auto& v = mImpl->m_listStore.find(key);
  if (v == mImpl->m_listStore.end()) {
    // try underlying store first
    if (mImpl->m_cachedStore) {
      return mImpl->m_cachedStore->get()->getKeyValueSet(key);
    }
    return std::make_unique<std::unordered_set<std::string>>(); // copy ctor
  }
  return std::make_unique<std::unordered_set<std::string>>(v->second); // copy ctor
}

void
MemoryKeyValueStore::loadKeysInto(std::function<void(std::string key,std::string value)> callback)
{
  for (auto element : mImpl->m_keyValueStore) {
    callback(element.first,element.second);
  }
  // TODO load indexes too???
}

void
MemoryKeyValueStore::clear()
{
  mImpl->m_keyValueStore.clear();
  if (mImpl->m_cachedStore) {
    mImpl->m_cachedStore->get()->clear();
  }
}

}
