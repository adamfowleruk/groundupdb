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

TEST_CASE("Key","[Key]") {

  // Story:-
  //   [Who]   As a database user
  //   [What]  I need to use keys of different types
  //   [Value] To fit my application data model and make implementation of my programmes easier
  SECTION("StringKey") {
    groundupdb::Key<std::string> k1("somekey");
    groundupdb::Key<std::string> k2("somekey");
    groundupdb::Key<std::string> k3("someotherkey");

    // test raw representation
    REQUIRE(k1.value() == k2.value());
    REQUIRE(k1.value() != k3.value());
    REQUIRE(k2.value() != k3.value());

    // now test binary representation
    char buffer1[15]{'0'};
    char buffer2[15]{'0'};
    char buffer3[15]{'0'};
    k1.copy_to(buffer1);
    k2.copy_to(buffer2);
    k3.copy_to(buffer3);

    REQUIRE(0 == strcmp(buffer1,buffer2));
    REQUIRE(0 != strcmp(buffer1,buffer3));
    REQUIRE(0 != strcmp(buffer2,buffer3));
  }

}
