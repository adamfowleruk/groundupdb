#ifndef GROUNDUPDB_H
#define GROUNDUPDB_H

#include <string>

#include "database.h"

class GroundUpDB
{
public:
  GroundUpDB();

  static Database createEmptyDB(std::string& dbname);
  static Database loadDB(std::string& dbname);
};

#endif // GROUNDUPDB_H
