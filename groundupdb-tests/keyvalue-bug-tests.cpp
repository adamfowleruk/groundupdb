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
#include "catch.hpp"

#include "groundupdb/groundupdb.h"

TEST_CASE("kv-bugs","[setKeyValue,getKeyValue]") {

  // BUG-0000001
  // Bug: When using a memory store to front a file store, and loading
  //      keys at second launch time, the kv file name was improperly
  //      constructed.
  // libc++abi.dylib: terminating with uncaught exception of type
  //   std::length_error: basic_string
  // [1]    96973 abort      ./groundupdb-cli -n mydb -s -k first -v value1
  SECTION("loadKeysIntoMemoryStoreFromFileStore_kvFilename") {
    std::string dbname("myemptydb");
    std::string key("simplestring");
    groundupdb::EncodedValue value("Some highly valuable value");
    {
      std::unique_ptr<groundupdb::IDatabase> db(
            groundupdb::GroundUpDB::createEmptyDB(dbname));

      // We know we have been successful when:-
      // 1. The retrieved value is the same as the stored value
      db->setKeyValue(key,std::move(value));
      REQUIRE(value == db->getKeyValue(key));
    }

    std::unique_ptr<groundupdb::IDatabase> db(
          groundupdb::GroundUpDB::loadDB(dbname));
    groundupdb::EncodedValue value2("Some highly valuable value number 2");
    REQUIRE_NOTHROW(db->setKeyValue(key,std::move(value2))); // BLOWS UP
    REQUIRE(value2 == db->getKeyValue(key));

    db->destroy();
  }
}
