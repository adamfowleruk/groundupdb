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
  void                        setKeyValue(std::string key,std::string value);
  std::string                 getKeyValue(std::string key);

  // Key-value management functions
  void                        loadKeysInto(std::function<void(std::string key,std::string value)> callback);
  void                        clear();

private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};

class FileKeyValueStore : public KeyValueStore {
public:
  FileKeyValueStore(std::string fullpath);
  ~FileKeyValueStore();

  // Key-Value use cases
  void                        setKeyValue(std::string key,std::string value);
  std::string                 getKeyValue(std::string key);

  void                        loadKeysInto(std::function<void(std::string key,std::string value)> callback);
  void                        clear();

private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};

class EmbeddedDatabase : public IDatabase {
public:
  EmbeddedDatabase(std::string dbname, std::string fullpath);
  EmbeddedDatabase(std::string dbname, std::string fullpath,
                   std::unique_ptr<KeyValueStore>& kvStore);
  ~EmbeddedDatabase();

  std::string                 getDirectory(void);

  // Key-Value use cases
  void                        setKeyValue(std::string key,std::string value);
  std::string                 getKeyValue(std::string key);

  // management functions
  static  const std::unique_ptr<IDatabase>    createEmpty(std::string dbname);
  static  const std::unique_ptr<IDatabase>    createEmpty(std::string dbname,std::unique_ptr<KeyValueStore>& kvStore);
  static  const std::unique_ptr<IDatabase>    load(std::string dbname);
  void                        destroy();

  class Impl;

private:
  std::unique_ptr<Impl> mImpl;
};

}

#endif // EXTDATABASE_H
