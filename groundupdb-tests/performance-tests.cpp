#include "catch.hpp"

#include "groundupdb/groundupdb.h"

TEST_CASE("Measure basic performance","[setKeyValue,getKeyValue]") {

  // Story:-
  //   [Who]   As a database administrator
  //   [What]  I need to know the performance characteristics of each GroundUpDB storage class
  //   [Value] So I can configure the most appropriate settings for each of my user's databases
  SECTION("Store and Retrieve 1 000 000 keys - Memory cached key-value store") {
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

    // 1. Pre-generate the keys and values in memory (so we don't skew the test)

    // 2. Store 1 000 000 key-value pairs (no overlap)
    // Raw storage speed

    // 3. Retrieve 100 000 key-value pairs (no overlap) (use % 10)
    // Raw retrieval speed

    // 4. Retrieve the same 100 000 key-value pairs
    // Retrieval speed with a 'warm cache' (if any implemented)

    // 5. Store 500 000 key-value pair UPDATES (so half of the data is 'new') (first half of db)
    // So the performance of half of the key-value pairs may differ

    // 6. Retrieve the same 100 000 key-value pairs
    // Same as 4 if we have a 'write through cache', halfway between 4 and 3 otherwise

    // We know we have been successful when:-
    // We have min,max,var,total time for each group of operations
    // We have saved these results to a csv file for later comparison

    // 7. Tear down
    db->destroy();
  }

  // Now do the same for pure disc backed storage
}
