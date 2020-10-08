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
#define CATCH_CONFIG_ENABLE_BENCHMARKING 1
#include "catch.hpp"

#include <unordered_map>
#include <iostream>
#include <chrono>
#include <string>

#include "groundupdb/groundupdb.h"
#include "groundupdb/groundupdbext.h"

TEST_CASE("Measure basic performance","[!hide][performance][setKeyValue][getKeyValue]") {

  // Story:-
  //   [Who]   As a database administrator
  //   [What]  I need to know the performance characteristics of each GroundUpDB storage class
  //   [Value] So I can configure the most appropriate settings for each of my user's databases
  SECTION("Store and Retrieve 100 000 keys - Memory cached key-value store") {
    std::cout << "====== Default key-value store performance test ======" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

    int total = 100'000;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::vector<std::pair<groundupdb::HashedKey,groundupdb::EncodedValue>> keyValues;
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    for (; i < total;i++) {
      keyValues.push_back(std::make_pair(groundupdb::HashedKey(std::to_string(i)),groundupdb::EncodedValue(std::to_string(i)))); // C++17, uses std::forward
    }
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    long every = 1000;
    // 2. Store 100 000 key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    i = 0;
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      db->setKeyValue(it->first,std::move(it->second));
      i++;
      if (0 == i % every) {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;
    std::cout << std::endl;

    // 3. Retrieve 100 000 key-value pairs (no overlap)
    // Raw retrieval speed
    std::string aString("blank");
    groundupdb::EncodedValue result(aString);
    std::cout << "====== GET ======" << std::endl;
    begin = std::chrono::steady_clock::now();
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      result = db->getKeyValue(it->first);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;



    // X. Retrieve 100 random keys
    //BENCHMARK("GET 100 random keys") {
    //  return db->getKeyValue(std::to_string(rand() % keyValues.size()));
    //};

    // 4. Retrieve the same 100 000 key-value pairs
    // Retrieval speed with a 'warm cache' (if any implemented)

    // 5. Store 50 000 key-value pair UPDATES (so half of the data is 'new') (first half of db)
    // So the performance of half of the key-value pairs may differ

    // 6. Retrieve the same 100 000 key-value pairs
    // Same as 4 if we have a 'write through cache', halfway between 4 and 3 otherwise

    // We know we have been successful when:-
    // We have min,max,var,total time for each group of operations
    // We have saved these results to a csv file for later comparison

    // 7. Tear down
    std::cout << "Tests complete" << std::endl;
    db->destroy();
  }


  SECTION("perf-100k-memory") {
    std::cout << "====== In-memory key-value store performance test ======" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::KeyValueStore> memoryStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::KeyValueStore> memoryIndexStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname, memoryStore, memoryIndexStore));

    int total = 100'000;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::vector<std::pair<groundupdb::HashedKey,groundupdb::EncodedValue>> keyValues;
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    for (; i < total;i++) {
      keyValues.push_back(std::make_pair(groundupdb::HashedKey(std::to_string(i)),groundupdb::EncodedValue(std::to_string(i)))); // C++17, uses std::forward
    }
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    long every = 1000;
    // 2. Store 100 000 key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    i = 0;
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      db->setKeyValue(it->first,std::move(it->second));
      i++;
      if (0 == i % every) {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;
    std::cout << std::endl;

    // 3. Retrieve 100 000 key-value pairs (no overlap)
    // Raw retrieval speed
    std::string aString("blank");
    groundupdb::EncodedValue result(aString);
    std::cout << "====== GET ======" << std::endl;
    begin = std::chrono::steady_clock::now();
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      result = db->getKeyValue(it->first);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;

    // 7. Tear down
    std::cout << "Tests complete" << std::endl;
    db->destroy();
  }
  
  // Now do the same for pure disc backed storage

  SECTION("Store and Retrieve 100 000 keys - File based key-value store") {
    std::cout << "====== File based key-value store performance test ======" << std::endl;
    std::string dbname("myemptydb");
    std::string fullpath = ".groundupdb/" + dbname;
    std::unique_ptr<groundupdb::KeyValueStore> memoryStore = std::make_unique<groundupdbext::FileKeyValueStore>(fullpath);
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname, memoryStore));

    int total = 100'000;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::vector<std::pair<groundupdb::HashedKey,groundupdb::EncodedValue>> keyValues;
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    for (; i < total;i++) {
      keyValues.push_back(std::make_pair(groundupdb::HashedKey(std::to_string(i)),groundupdb::EncodedValue(std::to_string(i)))); // C++17, uses std::forward
    }
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    long every = 1000;
    // 2. Store 100 000 key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    i = 0;
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      db->setKeyValue(it->first,std::move(it->second));
      i++;
      if (0 == i % every) {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;
    std::cout << std::endl;

    // 3. Retrieve 100 000 key-value pairs (no overlap)
    // Raw retrieval speed
    std::string aString("blank");
    groundupdb::EncodedValue result(aString);
    std::cout << "====== GET ======" << std::endl;
    begin = std::chrono::steady_clock::now();
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      result = db->getKeyValue(it->first);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;

    // 7. Tear down
    std::cout << "Tests complete" << std::endl;
    db->destroy();
  }
}

TEST_CASE("query-performance","[!hide][performance][query]") {

  SECTION("Bucket query performance test - In-memory key-value store") {
    std::cout << "====== In-memory key-value store performance test - Bucket query vs. key fetch ======" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::KeyValueStore> memoryStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::KeyValueStore> memoryIndexStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname, memoryStore, memoryIndexStore));

    int total = 1'000'000;
    int every =     1'000;
    std::string bucket("my bucket");

    std::unordered_set<std::string> keysInBuckets;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::vector<std::pair<groundupdb::HashedKey,groundupdb::EncodedValue>> keyValues;
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    for (; i < total;i++) {
      keyValues.push_back(std::make_pair(groundupdb::HashedKey(std::to_string(i)),groundupdb::EncodedValue(std::to_string(i)))); // C++17, uses std::forward
      if (0 == i%every) {
        keysInBuckets.insert(std::to_string(i));
      }
    }
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    // 2. Store key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    i=0;
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      if (0 == i%every) {
        db->setKeyValue(it->first,std::move(it->second),bucket);
      } else {
        db->setKeyValue(it->first,std::move(it->second));
      }
      i++;
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;
    std::cout << std::endl;

    // 3. Retrieve 100 000 key-value pairs (no overlap)
    // Raw retrieval speed
    std::string aString("blank");
    groundupdb::EncodedValue result(aString);
    std::cout << "====== GET KEYS IN THE BUCKET ======" << std::endl;
    begin = std::chrono::steady_clock::now();
    for (auto it = keysInBuckets.begin(); it != keysInBuckets.end(); it++) {
      result = db->getKeyValue(*it);
    }
    end = std::chrono::steady_clock::now();
    auto getTimeMicro = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0);
    std::cout << "  " << keysInBuckets.size() << " completed in "
              << getTimeMicro
              << " seconds" << std::endl;
    std::cout << "  "
              << (keysInBuckets.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;

    // 4. Query comparison
    std::cout << "====== QUERY KEYS IN THE BUCKET ======" << std::endl;
    groundupdb::BucketQuery bq(bucket);
    std::cout << "Executing query" << std::endl;
    begin = std::chrono::steady_clock::now();
    std::unique_ptr<groundupdb::IQueryResult> res(db->query(bq));
    std::cout << "Retrieving results" << std::endl;
    const groundupdb::KeySet& recordKeys = res->recordKeys();
    end = std::chrono::steady_clock::now();
    auto queryTimeMicro = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0);
    std::cout << "  Query completed in "
              << queryTimeMicro
              << " seconds" << std::endl;
    std::cout << "  "
              << (1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " queries per second" << std::endl;
    std::cout << "Asserting that the query result size should equal the number we placed in buckets" << std::endl;
    REQUIRE(recordKeys.get()->size() == keysInBuckets.size());
    std::cout << "Asserting that query time should be quicker than total get time" << std::endl;
    REQUIRE(queryTimeMicro < getTimeMicro);

    // 7. Tear down
    std::cout << "Tests complete" << std::endl;
    db->destroy();
  }
}

TEST_CASE("profiling-100k","[!hide][performance][memory][100k]") {
  
  SECTION("perf-100k-memory") {
    std::cout << "====== In-memory key-value store performance test - 100k ======" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::KeyValueStore> memoryStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::KeyValueStore> memoryIndexStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname, memoryStore, memoryIndexStore));

    int total = 100'000;
    long every = 1'000;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::vector<std::pair<groundupdb::HashedKey,groundupdb::EncodedValue>> keyValues;
    keyValues.reserve(total);
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (; i < total;i++) {
      keyValues.push_back(std::make_pair(groundupdb::HashedKey(std::to_string(i)),groundupdb::EncodedValue(std::to_string(i)))); // C++17, uses std::forward
      if (0 == i % every) {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;
    std::cout << std::endl;
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    // 2. Store key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    begin = std::chrono::steady_clock::now();
    i = 0;
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      db->setKeyValue(it->first,std::move(it->second));
      i++;
      if (0 == i % every) {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
    end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;
    std::cout << std::endl;

    // 3. Retrieve key-value pairs (no overlap)
    // Raw retrieval speed
    std::string aString("blank");
    groundupdb::EncodedValue result(aString);
    std::cout << "====== GET ======" << std::endl;
    begin = std::chrono::steady_clock::now();
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      result = db->getKeyValue(it->first);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;

    // 7. Tear down
    std::cout << "Tests complete" << std::endl;
    db->destroy();
  }
}

TEST_CASE("profiling-1m","[!hide][performance][memory][1m]") {
  
  SECTION("perf-1m-memory") {
    std::cout << "====== In-memory key-value store performance test - 1m ======" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::KeyValueStore> memoryStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::KeyValueStore> memoryIndexStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname, memoryStore, memoryIndexStore));

    int total = 1'000'000;
    long every = 10'000;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::vector<std::pair<groundupdb::HashedKey,groundupdb::EncodedValue>> keyValues;
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (; i < total;i++) {
      keyValues.push_back(std::make_pair(groundupdb::HashedKey(std::to_string(i)),groundupdb::EncodedValue(std::to_string(i)))); // C++17, uses std::forward
      if (0 == i % every) {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;
    std::cout << std::endl;
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    // 2. Store key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    begin = std::chrono::steady_clock::now();
    i = 0;
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      db->setKeyValue(it->first,std::move(it->second));
      i++;
      if (0 == i % every) {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
    end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;
    std::cout << std::endl;

    // 3. Retrieve key-value pairs (no overlap)
    // Raw retrieval speed
    std::string aString("blank");
    groundupdb::EncodedValue result(aString);
    std::cout << "====== GET ======" << std::endl;
    begin = std::chrono::steady_clock::now();
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      result = std::move(db->getKeyValue(it->first));
    }
    end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0)
              << " seconds" << std::endl;
    std::cout << "  "
              << (keyValues.size() * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
              << " requests per second" << std::endl;

    // 7. Tear down
    std::cout << "Tests complete" << std::endl;
    db->destroy();
  }

}