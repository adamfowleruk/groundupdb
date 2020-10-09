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

TEST_CASE("hashedvalue","[types][hashedvalue]") {

  SECTION("string") {
    std::string str("some unique and amazing string");
    std::string str2("some unique and amazing string");
    groundupdb::HashedValue ev1(str);
    groundupdb::HashedValue ev2(str2);

    REQUIRE(0 != str.length()); // ensure it hasn't had its contents moved
    REQUIRE(0 != str2.length()); // ensure it hasn't had its contents moved
    REQUIRE(str.length() == str2.length()); // idiot check
    REQUIRE(ev1.hasValue());
    REQUIRE(ev2.hasValue());
    REQUIRE(str.length() == ev1.length());
    REQUIRE(str.length() == ev2.length());
    char ev1chars[ev1.length()];
    int pos = 0;
    for (auto& c : ev1.data()) {
      ev1chars[pos++] = (const char)c;
    }
    char ev2chars[ev2.length()];
    pos = 0;
    for (auto& c : ev2.data()) {
      ev2chars[pos++] = (const char)c;
    }
    //REQUIRE(0 == strcmp(str.c_str(),ev1.data()));
    //std::cout << "ev1 data: '" << ev1.data() << "', ev2 data: '" << ev2.data() << "'" << std::endl;
    REQUIRE(0 == strcmp(ev1chars,ev2chars));
    REQUIRE(0 != ev1.hash());
    REQUIRE(0 != ev2.hash());
    REQUIRE(ev1.hash() == ev2.hash());

    // elements of == function
    REQUIRE(ev1.hash()==ev2.hash());
    REQUIRE(ev1.hasValue()==ev2.hasValue());
    REQUIRE(ev1.length()==ev2.length());
    REQUIRE(0==strcmp(ev1chars,ev2chars));
    // actual == function
    REQUIRE(ev1 == ev2);

    REQUIRE(8 == sizeof(ev1.hash()));
    REQUIRE(8 == sizeof(ev2.hash()));
  }


}
