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

TEST_CASE("Store and retrieve a value","[setKeyValue,getKeyValue]") {

  // Story:-
  //   [Who]   As a database user
  //   [What]  I need to store and retrieve a value with a simple name
  //   [Value] So I can persist data for later use
  SECTION("Basic set and get") {
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value
    std::string key("simplestring");
    std::string value("Some highly valuable value");
    db->setKeyValue(key,value);
    REQUIRE(value == db->getKeyValue(key));

    std::string value2("Some highly valuable value number 2");
    db->setKeyValue(key,value2);
    REQUIRE(value2 == db->getKeyValue(key));

    db->destroy();
  }

  //   [Who]   As a database user
  //   [What]  I want to be able to logically segment my data
  //   [Value] To make storage, querying, and retrieval easier and quicker
  SECTION("Bucketed set and get") {
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value
    std::string key("simplestring");
    std::string value("Some highly valuable value");
    std::string bucket("bucket 1");
    db->setKeyValue(key,value, bucket);
    REQUIRE(value == db->getKeyValue(key));

    // Note bucket query is in the query-tests.cpp file

    db->destroy();
  }


}

TEST_CASE("Store and retrieve a list","setKeyValue(std::string,std::unordered_set<std::string>),getKeyValueSet") {

  //   [Who]   As a database user
  //   [What]  I want to be able to store multiple values against a single key (i.e. a set or list)
  //   [Value] To fit the data model I need in my application
  SECTION("Basic List set and get") {
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value
    std::string key("simplestring");
    std::string v1("Some highly valuable value");
    std::string v2("Some highly valuable value 2");
    std::string v3("Some highly valuable value 3");
    std::unordered_set<std::string> set;
    set.insert(v1);
    set.insert(v2);
    set.insert(v3);
    db->setKeyValue(key,set);
    auto result = db->getKeyValueSet(key);
    REQUIRE(result.get()->size() == 3);
    REQUIRE(result.get()->find(v1) != result.get()->end());
    REQUIRE(result.get()->find(v2) != result.get()->end());
    REQUIRE(result.get()->find(v3) != result.get()->end());

    db->destroy();
  }
}
