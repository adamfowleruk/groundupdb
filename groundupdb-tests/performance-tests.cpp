#define CATCH_CONFIG_ENABLE_BENCHMARKING 1
#include "catch.hpp"

#include <unordered_map>
#include <iostream>
#include <chrono>
#include <string>

#include "groundupdb/groundupdb.h"
#include "groundupdb/groundupdbext.h"

TEST_CASE("Measure basic performance","[setKeyValue,getKeyValue]") {

  // Story:-
  //   [Who]   As a database administrator
  //   [What]  I need to know the performance characteristics of each GroundUpDB storage class
  //   [Value] So I can configure the most appropriate settings for each of my user's databases
  SECTION("Store and Retrieve 100 000 keys - Memory cached key-value store") {
    std::cout << "====== Default key-value store performance test ======" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

    int total = 100000;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::unordered_map<std::string,std::string> keyValues;
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    for (; i < total;i++) {
      keyValues.emplace(std::to_string(i),std::to_string(i)); // C++11, uses std::forward
    }
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    // 2. Store 100 000 key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      db->setKeyValue(it->first,it->second);
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
    std::string& result(aString);
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


  SECTION("Store and Retrieve 100 000 keys - In-memory key-value store") {
    std::cout << "====== In-memory key-value store performance test ======" << std::endl;
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::KeyValueStore> memoryStore = std::make_unique<groundupdbext::MemoryKeyValueStore>();
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname, memoryStore));

    int total = 100000;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::unordered_map<std::string,std::string> keyValues;
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    for (; i < total;i++) {
      keyValues.emplace(std::to_string(i),std::to_string(i)); // C++11, uses std::forward
    }
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    // 2. Store 100 000 key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      db->setKeyValue(it->first,it->second);
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
    std::string& result(aString);
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

    int total = 100000;

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)
    std::unordered_map<std::string,std::string> keyValues;
    long i = 0;
    std::cout << "Pre-generating key value pairs..." << std::endl;
    for (; i < total;i++) {
      keyValues.emplace(std::to_string(i),std::to_string(i)); // C++11, uses std::forward
    }
    std::cout << "Key size is max " << std::to_string(total - 1).length() << " bytes" << std::endl;

    // 2. Store 100 000 key-value pairs (no overlap)
    // Raw storage speed
    std::cout << "====== SET ======" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (auto it = keyValues.begin(); it != keyValues.end(); it++) {
      db->setKeyValue(it->first,it->second);
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
    std::string& result(aString);
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
