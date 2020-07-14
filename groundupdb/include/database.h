#ifndef DATABASE_H
#define DATABASE_H

#include <string>

namespace groundupdb {

class Store {
public:
  Store() = default;
  virtual ~Store() = default;
};

class KeyValueStore : public Store {
public:
  KeyValueStore() = default;
  virtual ~KeyValueStore() = default;

  // Key-Value user functions
  virtual void                        setKeyValue(std::string key,std::string value) = 0;
  virtual std::string                 getKeyValue(std::string key) = 0;

  // Key-value management functions
  virtual void                        loadKeysInto(std::function<void(std::string key,std::string value)> callback) = 0;
  virtual void                        clear() = 0;
};

class IDatabase
{
public:
  IDatabase() = default;
  virtual ~IDatabase() = default;
  virtual std::string           getDirectory(void) = 0;

  // Key-Value use cases
  virtual void                  setKeyValue(std::string key,std::string value) = 0;
  virtual std::string           getKeyValue(std::string key) = 0;

  // management functions
  static const std::unique_ptr<IDatabase>       createEmpty(std::string dbname);
  static const std::unique_ptr<IDatabase>       createEmpty(std::string dbname,std::unique_ptr<KeyValueStore>& kvStore);
  static const std::unique_ptr<IDatabase>       load(std::string dbname);
  virtual void                  destroy() = 0;

};

}

#endif // DATABASE_H
