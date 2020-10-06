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
#include "tests.h"

#include <filesystem>
#include <string>

#include "groundupdb/groundupdb.h"

namespace fs = std::filesystem;

TEST_CASE("db-create","[createEmptyDB]") {

    // Story:-
    //   [Who]   As a database administrator
    //   [What]  I need to create a new empty database
    //   [Value] So I can later store and retrieve data

    SECTION("Default settings") {
        std::string dbname("myemptydb");
        std::cout << "creating db" << std::endl;
        std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

        // We know we have been successful when:-
        // 1. We have a valid database reference returned
        //   - No test -> The above would have errored
        // 2. The DB has a folder that exists on the file system
        std::cout << "checking db directory" << std::endl;
        REQUIRE(fs::is_directory(fs::status(db->getDirectory())));
        // C++17 Ref: https://en.cppreference.com/w/cpp/filesystem/is_directory

        // 3. The database folder is empty (no database files yet)

        std::cout << "iterating over db directory" << std::endl;
        auto p = fs::directory_iterator(db->getDirectory());
        REQUIRE(p != end(p)); // Should have an empty .indexes folder
        REQUIRE(p->is_directory()); // The .indexes folder
        p++;
        REQUIRE(p == end(p)); // i.e. no contents as iterator is at the end already

        // C++17 Ref: https://en.cppreference.com/w/cpp/filesystem/directory_iterator

        std::cout << "destroying db" << std::endl;
        db->destroy();
        std::cout << "checking destroyed db directory" << std::endl;
        REQUIRE(!fs::exists(fs::status(db->getDirectory())));
    }
}

TEST_CASE("db-load","[loadDB]") {

    // Story:-
    //   [Who]   As a database user
    //   [What]  I need to create a reference to an existing database
    //   [Value] So I can later store and retrieve data

    SECTION("Default settings") {
        std::string dbname("myemptydb");
        std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

        std::unique_ptr<groundupdb::IDatabase> db2(groundupdb::GroundUpDB::loadDB(dbname));

        // We know we have been successful when:-
        // 1. We have a valid database reference returned
        //   - No test -> The above would have errored
        // 2. The DB has a folder that exists on the file system
        REQUIRE(fs::is_directory(fs::status(db2->getDirectory())));
        // C++17 Ref: https://en.cppreference.com/w/cpp/filesystem/is_directory

        // 3. The database folder is empty (no database files yet)
        auto p = fs::directory_iterator(db->getDirectory());
        REQUIRE(p != end(p)); // Should have an empty .indexes folder
        REQUIRE(p->is_directory()); // The .indexes folder
        p++;
        REQUIRE(p == end(p)); // i.e. no contents as iterator is at the end already

        // C++17 Ref: https://en.cppreference.com/w/cpp/filesystem/directory_iterator

        // Clear out the DB
        db2->destroy();
        REQUIRE(!fs::exists(fs::status(db2->getDirectory())));
    }
}

