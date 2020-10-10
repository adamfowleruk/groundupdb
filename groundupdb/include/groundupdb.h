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
#ifndef GROUNDUPDB_H
#define GROUNDUPDB_H

#include <string>

// WARNING: This should ONLY include Client API files
// i.e. NOT anything within include/extensions!

#include "database.h"
#include "hashes.h"
#include "is_container.h"
#include "query.h"
#include "types.h"

namespace groundupdb {

class GroundUpDB {
 public:
  GroundUpDB();

  static std::unique_ptr<IDatabase> createEmptyDB(std::string& dbname);
  static std::unique_ptr<IDatabase> createEmptyDB(
      std::string& dbname, std::unique_ptr<KeyValueStore>& kvStore);
  static std::unique_ptr<IDatabase> createEmptyDB(
      std::string& dbname, std::unique_ptr<KeyValueStore>& kvStore,
      std::unique_ptr<KeyValueStore>& idxStore);
  static std::unique_ptr<IDatabase> loadDB(std::string& dbname);
};

}  // namespace groundupdb

#endif  // GROUNDUPDB_H
