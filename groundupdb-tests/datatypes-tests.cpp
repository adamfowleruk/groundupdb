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

template <typename T>
void check(const std::unique_ptr<groundupdb::IDatabase>& db, T v)
{
  std::string k("testkey");
  groundupdb::EncodedValue ev(v);
  db->setKeyValue(k, std::move(ev));
  groundupdb::EncodedValue evr = db->getKeyValue(k);
  REQUIRE(ev.hasValue());
  REQUIRE(evr.hasValue());
  REQUIRE(0 != ev.hash());
  REQUIRE(0 != evr.hash());
  REQUIRE(ev.hash() == evr.hash());
  REQUIRE(0 != ev.length());
  REQUIRE(0 != evr.length());
  REQUIRE(ev.length() == evr.length());
}

TEST_CASE("datatypes-memory", "[datatypes][memory]")
{

  // Story:-
  //   [Who]   As an app programmer
  //   [What]  I need to store and retrieve a variety of data types
  //   [Value] So I can persist data for later use in my app
  SECTION("datatypes-memory")
  {
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(groundupdb::GroundUpDB::createEmptyDB(dbname));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value
    std::string vstring("Some highly valuable value");

    // test of all equivalent types from:
    // https://en.cppreference.com/w/cpp/language/types
    check(db, (signed char)'b');
    check(db, (unsigned char)'b');
    check(db, (char)'b');
    check(db, (wchar_t)'b');
    check(db, (char16_t)'b');
    check(db, (char32_t)'b');
    check(db, "onetwothree");
    check(db, vstring);
    check(db, (short int)147);
    check(db, (unsigned short int)147);
    check(db, (int)147);
    check(db, (unsigned int)147);
    check(db, (long int)147);
    check(db, (unsigned long int)147);
    check(db, (long long int)147);
    check(db, (unsigned long long int)147);
    check(db, (std::size_t)147);
    check(db, (float)3.1415927);
    check(db, (double)3.1415927);
    check(db, (long double)3.1415927);

    db->destroy();
  }

}
