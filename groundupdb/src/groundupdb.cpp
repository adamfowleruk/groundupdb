#include "groundupdb.h"
#include "extensions/extdatabase.h"

using namespace groundupdb;
using namespace groundupdbext;

GroundUpDB::GroundUpDB()
{
}

std::unique_ptr<IDatabase> GroundUpDB::createEmptyDB(std::string &dbname) {
  return EmbeddedDatabase::createEmpty(dbname);
}

std::unique_ptr<IDatabase> GroundUpDB::createEmptyDB(std::string &dbname,std::unique_ptr<KeyValueStore>& kvStore) {
  return EmbeddedDatabase::createEmpty(dbname,kvStore);
}

std::unique_ptr<IDatabase> GroundUpDB::loadDB(std::string &dbname) {
  return EmbeddedDatabase::load(dbname);
}
