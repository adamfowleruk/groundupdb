#ifndef GROUNDUPDB_H
#define GROUNDUPDB_H

#include <string>

// WARNING: This should ONLY include Client API files
// i.e. NOT anything within include/extensions!

#include "database.h"

namespace groundupdb {

class GroundUpDB
{
public:
  GroundUpDB();

  static std::unique_ptr<IDatabase> createEmptyDB(std::string& dbname);
  static std::unique_ptr<IDatabase> loadDB(std::string& dbname);
};

}

#endif // GROUNDUPDB_H
