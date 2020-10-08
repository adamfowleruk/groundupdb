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
#include "groundupdb/groundupdbext.h"

#include <iostream>

TEST_CASE("query","[query]") {

  //   [Who]   As a database user
  //   [What]  I want to be able to logically segment my data
  //   [Value] To make storage, querying, and retrieval easier and quicker
  SECTION("query-memorystore") {
    std::cout << "Creating DB" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value
    std::cout << "Setting keys" << std::endl;
    std::string key("simplestring");
    groundupdb::EncodedValue value("Some highly valuable value");
    std::string key2("second string");
    groundupdb::EncodedValue value2("second valuable value");
    std::string key3("third string");
    groundupdb::EncodedValue value3("third valuable value");
    std::string key4("fourth string");
    groundupdb::EncodedValue value4("fourth valuable value");
    std::string bucket("bucket 1");
    db->setKeyValue(key,std::move(value),bucket);
    db->setKeyValue(key2,std::move(value2));
    db->setKeyValue(key3,std::move(value3));
    db->setKeyValue(key4,std::move(value4),bucket);

    // Note set with bucket proven in keyvalue-tests.cpp

    // Now add the bucket query
    std::cout << "Creating query" << std::endl;
    std::string queryBucket("bucket 1"); // second definition to ensure no weird pointer serialisation
    groundupdb::BucketQuery bq(queryBucket);
    std::cout << "Executing query" << std::endl;
    std::unique_ptr<groundupdb::IQueryResult> res = db->query(bq);
    std::cout << "Retrieving results" << std::endl;
    const groundupdb::KeySet& recordKeys = res->recordKeys();

    std::cout << "Processing results" << std::endl;
    std::vector<std::string> keys;
    for (auto it = recordKeys->begin(); it != recordKeys->end();it++) {
      std::cout << "  Getting result data" << std::endl;
      groundupdb::Bytes bytes = it->data();
      std::cout << "  Creating char array" << std::endl;
      char* chars = new char[bytes.size() + 1];
      int pos = 0;
      std::cout << "  Filling char array" << std::endl;
      for (auto& c : bytes) {
        chars[pos++] = (char)c;
      }
      chars[pos] = '\0';
      std::cout << "  Creating string" << std::endl;
      std::string k(chars);
      INFO("  Key in bucket: " << k);
      keys.push_back(k);
    }

    std::cout << "Asserting size" << std::endl;
    REQUIRE(recordKeys->size() == 2);
    std::cout << "Asserting key 1 found" << std::endl;
    REQUIRE(recordKeys->find(key) != recordKeys->end());
    std::cout << "Asserting key 4 found" << std::endl;
    REQUIRE(recordKeys->find(key4) != recordKeys->end());
    std::cout << "Asserting key 2 NOT found" << std::endl;
    REQUIRE(recordKeys->find(key2) == recordKeys->end());
    std::cout << "Asserting key 3 NOT found" << std::endl;
    REQUIRE(recordKeys->find(key3) == recordKeys->end());

    // Ensure result keys are the same as those set for the bucket
    REQUIRE(keys.size() == 2);
    REQUIRE((keys[0] == key || keys[1] == key) & (keys[0] == key4 || keys[1] == key4));

    std::cout << "Destroying db" << std::endl;
    db->destroy();
  }

  SECTION("query-filestore") {
    std::cout << "Creating DB" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::KeyValueStore> kvs = std::make_unique<groundupdbext::FileKeyValueStore>("./groundupdb/" + dbname);
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname,kvs));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value
    std::cout << "Setting keys" << std::endl;
    std::string key("simplestring");
    groundupdb::EncodedValue value("Some highly valuable value");
    std::string key2("second string");
    groundupdb::EncodedValue value2("second valuable value");
    std::string key3("third string");
    groundupdb::EncodedValue value3("third valuable value");
    std::string key4("fourth string");
    groundupdb::EncodedValue value4("fourth valuable value");
    std::string bucket("bucket 1");
    db->setKeyValue(key,std::move(value),bucket);
    db->setKeyValue(key2,std::move(value2));
    db->setKeyValue(key3,std::move(value3));
    db->setKeyValue(key4,std::move(value4),bucket);

    // Note set with bucket proven in keyvalue-tests.cpp

    // Now add the bucket query
    std::cout << "Creating query" << std::endl;
    std::string queryBucket("bucket 1"); // second definition to ensure no weird pointer serialisation
    groundupdb::BucketQuery bq(queryBucket);
    std::cout << "Executing query" << std::endl;
    std::unique_ptr<groundupdb::IQueryResult> res = db->query(bq);
    std::cout << "Retrieving results" << std::endl;
    const groundupdb::KeySet& recordKeys = res->recordKeys();

    std::cout << "Processing results" << std::endl;
    std::vector<std::string> keys;
    for (auto it = recordKeys->begin(); it != recordKeys->end();it++) {
      std::cout << "  Getting result data" << std::endl;
      groundupdb::Bytes bytes = it->data();
      std::cout << "  Creating char array" << std::endl;
      char* chars = new char[bytes.size() + 1];
      int pos = 0;
      std::cout << "  Filling char array" << std::endl;
      for (auto& c : bytes) {
        chars[pos++] = (char)c;
      }
      chars[pos] = '\0';
      std::cout << "  Creating string" << std::endl;
      std::string k(chars);
      INFO("  Key in bucket: " << k);
      keys.push_back(k);
    }

    std::cout << "Asserting size" << std::endl;
    REQUIRE(recordKeys->size() == 2);
    std::cout << "Asserting key 1 found" << std::endl;
    REQUIRE(recordKeys->find(key) != recordKeys->end());
    std::cout << "Asserting key 4 found" << std::endl;
    REQUIRE(recordKeys->find(key4) != recordKeys->end());
    std::cout << "Asserting key 2 NOT found" << std::endl;
    REQUIRE(recordKeys->find(key2) == recordKeys->end());
    std::cout << "Asserting key 3 NOT found" << std::endl;
    REQUIRE(recordKeys->find(key3) == recordKeys->end());

    // Ensure result keys are the same as those set for the bucket
    REQUIRE(keys.size() == 2);
    REQUIRE((keys[0] == key || keys[1] == key) & (keys[0] == key4 || keys[1] == key4));

    std::cout << "Destroying db" << std::endl;
    db->destroy();
  }

}
