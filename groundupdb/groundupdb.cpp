#include "groundupdb.h"

GroundUpDB::GroundUpDB()
{
}

Database GroundUpDB::createEmptyDB(std::string &dbname) {
  return Database::createEmpty(dbname);
}

Database GroundUpDB::loadDB(std::string &dbname) {
  return Database::load(dbname);
}
