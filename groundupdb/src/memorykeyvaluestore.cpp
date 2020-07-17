#include "extensions/extdatabase.h"
#include "extensions/highwayhash.h"

#include <unordered_map>

namespace groundupdbext {

class MemoryKeyValueStore::Impl {
public:
  Impl();
  Impl(std::unique_ptr<KeyValueStore>& toCache);

  std::unordered_map<std::string,std::string,HighwayHash> m_keyValueStore;
  std::optional<std::unique_ptr<KeyValueStore>> m_cachedStore;

private:

};

MemoryKeyValueStore::Impl::Impl()
  : m_keyValueStore(), m_cachedStore()
{
  ;
}

MemoryKeyValueStore::Impl::Impl(std::unique_ptr<KeyValueStore>& toCache)
  : m_keyValueStore(), m_cachedStore(toCache.release())
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
  mImpl->m_keyValueStore.insert({key,value});
  if (mImpl->m_cachedStore) {
    mImpl->m_cachedStore->get()->setKeyValue(key,value);
  }
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
MemoryKeyValueStore::loadKeysInto(std::function<void(std::string key,std::string value)> callback)
{
  for (auto element : mImpl->m_keyValueStore) {
    callback(element.first,element.second);
  }
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
