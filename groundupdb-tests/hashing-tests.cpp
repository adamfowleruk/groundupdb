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
#include "highwayhash/highwayhash.h"

#include <unordered_map>

TEST_CASE("Hashing","[set,get]") {

  // Story #7 on GitHub
  //   [Who]   As a database user
  //   [What]  I want to be able to logically segment my data
  //   [Value] To make storage, querying, retrieval, backups and restores easier and quicker
  SECTION("Produces known expected value for known input") {
    groundupdbext::HighwayHash h;
    std::string text("Known");
    std::size_t r = h(text);

    using namespace highwayhash;
    const HHKey key HH_ALIGNAS(64) = {1, 2, 3, 4};
    HHResult64 result;  // or HHResult128 or HHResult256
    HHStateT<HH_TARGET> state(key);
    HighwayHashT(&state, text.c_str(), text.length(), &result);

    REQUIRE(result == r);
  }

  SECTION("Previous hash doesn't affect next hash result") {
    groundupdbext::HighwayHash hfirst;
    std::size_t rfirst = hfirst(std::string("OtherThing"));

    groundupdbext::HighwayHash h;
    std::size_t r1 = h(std::string("Known"));
    std::size_t r2 = h(std::string("OtherThing"));

    REQUIRE(r1 != r2);
    REQUIRE(r1 != rfirst);
    REQUIRE(rfirst == r2);
  }

  SECTION("Different seed keys produce different results for the same input") {
    groundupdbext::HighwayHash h1{1,2,3,4};
    std::size_t r1 = h1(std::string("AThing"));

    groundupdbext::HighwayHash h2{5,6,7,8};
    std::size_t r2 = h2(std::string("AThing"));

    REQUIRE(r1 != r2);
  }

  SECTION("Unordered map works as expected with custom hash") {
    std::unordered_map<std::string,std::string,groundupdbext::HighwayHash> m;
    std::string key("A very sensible key name");
    std::string value("A very valuable value");
    std::string k2("Some other key");
    std::string v2("Another value");
    m.emplace(key,value);
    m.emplace(k2,v2);

    REQUIRE(m[key] == value);
    REQUIRE(m[k2] == v2);
  }

  SECTION("StringKeyHashing") {
    groundupdb::Key<std::string> k1("somekey");
    groundupdb::Key<std::string> k2("somekey");
    groundupdb::Key<std::string> k3("someotherkey");
    std::string buffer1("somekey");
    std::string buffer2("somekey");
    std::string buffer3("someotherkey");
    groundupdbext::HighwayHash h1{1,2,3,4};
    std::size_t hv1 = h1(buffer1);
    std::size_t hv2 = h1(buffer2);
    std::size_t hv3 = h1(buffer3);

    REQUIRE(hv1 == hv2);
    REQUIRE(hv1 != hv3);
    REQUIRE(hv2 != hv3);
    REQUIRE(8 == sizeof (hv1));
    REQUIRE(8 == sizeof (hv2));
    REQUIRE(8 == sizeof (hv3));
  }

}
