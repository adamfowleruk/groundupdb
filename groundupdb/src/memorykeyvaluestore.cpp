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
#include <optional>

namespace groundupdbext {

class MemoryKeyValueStore::Impl {
public:
  Impl();
  Impl(std::unique_ptr<KeyValueStore>& toCache);

  std::unordered_map<HashedValue,EncodedValue,HighwayHash> m_keyValueStore;
  std::unordered_map<HashedValue,Set,HighwayHash> m_listStore;
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
  mImpl->m_cachedStore->get()->loadKeysInto([this](const HashedValue& key,EncodedValue value) {
    mImpl->m_keyValueStore.insert({key,value});
  });
}


MemoryKeyValueStore::~MemoryKeyValueStore()
{
  ;
}

// Key-Value use cases
void
MemoryKeyValueStore::setKeyValue(const HashedValue& key,EncodedValue&& value)
{
  // Also write to our in-memory unordered map
  // Note: insert on unordered_map does NOT perform an insert if the key already exists
  mImpl->m_keyValueStore.erase(key);
  mImpl->m_keyValueStore.insert({key,value});
  if (mImpl->m_cachedStore) {
    mImpl->m_cachedStore->get()->setKeyValue(key,EncodedValue(value)); // force copy construction of a temporary
  }
}

EncodedValue
MemoryKeyValueStore::getKeyValue(const HashedValue& key)
{
  // Only ever read from our in memory map!
  const auto& v = mImpl->m_keyValueStore.find(key);
  if (v == mImpl->m_keyValueStore.end()) {
    return EncodedValue(); // Now provides an empty value with hasValue() == false
    // TODO make the above more efficient - no construct-then-copy
  }
  return v->second;
}


void
MemoryKeyValueStore::setKeyValue(const HashedValue& key,const Set& value) {
  // Note: insert on unordered_map does NOT perform an insert if the key already exists
  mImpl->m_listStore.erase(key);
  Set newvalue = std::make_unique<std::unordered_set<EncodedValue>>(); // WARNING: MUST use make_unique here!

  //std::cout << "MEMKVS: Inserting set values in to copy of set" << std::endl;
  //newvalue->merge(*value);
  newvalue->insert(value->begin(),value->end());
  //std::cout << "MEMKVS: Set size now: " << newvalue->size() << std::endl;
  //mImpl->m_listStore.insert({key,newvalue}); // STD LIB bug. See https://stackoverflow.com/questions/14808663/stdunordered-mapemplace-issue-with-private-deleted-copy-constructor
  //std::cout << "MEMKVS: emplacing new set" << std::endl;
  mImpl->m_listStore.emplace(key,std::forward<Set>(newvalue));
  //mImpl->m_listStore.emplace(key,value);
  //std::cout << "MEMKVS: Checking cache" << std::endl;
  if (mImpl->m_cachedStore) {
    //std::cout << "MEMKVS: Setting cache value" << std::endl;
    mImpl->m_cachedStore->get()->setKeyValue(key,value);
  }
}

Set
MemoryKeyValueStore::getKeyValueSet(const HashedValue& key) {
  const auto& v = mImpl->m_listStore.find(key);
  //std::cout << "MEMKVS: getKeyValueSet" << std::endl;
  if (v == mImpl->m_listStore.end()) {
    //std::cout << "  MEMKVS: not found in memory" << std::endl;
    // try underlying store first
    if (mImpl->m_cachedStore) {
      //std::cout << "  MEMKVS: checking underlying store we're caching for and returning" << std::endl;
      return mImpl->m_cachedStore->get()->getKeyValueSet(key);
    }
    //std::cout << "  MEMKVS: Returning empty value" << std::endl;
    return std::make_unique<std::unordered_set<EncodedValue>>(); // copy ctor
  }
  //std::cout << "  MEMKVS: returning Set with size: " << v->second->size() << std::endl;
  // Note we dereference the below because v->second is an unorderedmap hashedvalue wrapper, and not the Set value itself
  return std::make_unique<std::unordered_set<EncodedValue>>(*(v->second)); // copy ctor
}

void
MemoryKeyValueStore::loadKeysInto(std::function<void(const HashedValue& key,EncodedValue value)> callback)
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
