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
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "catch.hpp"
#include "groundupdb/groundupdb.h"

class MyCustomType {
public:
  MyCustomType() = default;
  MyCustomType(const std::string& toCopy) : m_storage(toCopy) { };
  MyCustomType(const MyCustomType& toCopy) = default;
  MyCustomType(MyCustomType&& toMove) = default;
  ~MyCustomType() = default;

  operator groundupdb::HashedValue() {
    groundupdb::HashedValue hv(m_storage);

    return std::move(hv);
  }

  operator groundupdb::EncodedValue() {
    groundupdb::EncodedValue ev(m_storage);

    return std::move(ev);
  }

private:
  std::string m_storage;
};

template <typename T>
void check(const std::unique_ptr<groundupdb::IDatabase> &db, T v) {
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

TEST_CASE("datatypes-basic-memory", "[datatypes][basic][memory]") {
  // Story:-
  //   [Who]   As an app programmer
  //   [What]  I need to store and retrieve a variety of data types
  //   [Value] So I can persist data for later use in my app
  SECTION("datatypes-intrinsic-memory") {
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(
        groundupdb::GroundUpDB::createEmptyDB(dbname));

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

TEST_CASE("datatypes-container-memory", "[datatypes][container][memory]") {
  // Story:-
  //   [Who]   As an app programmer
  //   [What]  I need to store and retrieve a variety of data types
  //   [Value] So I can persist data for later use in my app
  SECTION("datatypes-container-memory") {
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(
        groundupdb::GroundUpDB::createEmptyDB(dbname));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value

    const int total = 100;

    // Containers from here (STL)
    // https://en.cppreference.com/w/cpp/container

    // Simplest example - Bytes (the wrapped value inside HashedValue)
    std::byte b = std::byte('b');
    groundupdb::Bytes bytes; // aka std::vector<std::byte>
    for (int i = 0; i < total; i++) {
      bytes.push_back(b); // copy ctor
    }
    check(db, bytes);

    // A sequence
    std::vector<std::string> list;
    for (int i = 0; i < total; i++) {
      list.push_back(std::to_string(i));
    }
    check(db, list);

    std::array<int, 100> arr;
    for (int i = 0; i < total; i++) {
      arr[i] = i;
    }
    check(db, arr);

    std::deque<std::string> deque;
    for (int i = 0; i < total; i++) {
      deque.insert(std::end(deque), std::to_string(i));
    }
    check(db, deque);

    std::forward_list<std::string> fwl;
    for (int i = 0; i < total; i++) {
      fwl.push_front(std::to_string(i));
    }
    check(db, fwl);

    std::list<std::string> lst;
    for (int i = 0; i < total; i++) {
      lst.push_back(std::to_string(i));
    }
    check(db, lst);

    // B Associative
    std::set<std::string> set;
    for (int i = 0; i < total; i++) {
      set.insert(std::to_string(i));
    }
    check(db, set);

    std::map<int, std::string> map;
    for (int i = 0; i < total; i++) {
      map.emplace(i, std::to_string(i));  // template ctor
    }
    check(db, map);

    std::multiset<std::string> mset;
    for (int i = 0; i < total; i++) {
      mset.insert(std::to_string(i));
    }
    check(db, mset);

    std::multimap<int, std::string> mmap;
    for (int i = 0; i < total; i++) {
      mmap.emplace(i % (total / 2),
                   std::to_string(i % (total / 2)));  // template ctor
      // ^^^ testing multiple instances of same key to be sure
    }
    check(db, mmap);

    // C Unordered associative
    std::unordered_set<std::string> uset;
    for (int i = 0; i < total; i++) {
      uset.insert(std::to_string(i));
    }
    check(db, uset);

    std::unordered_map<int, std::string> umap;
    for (int i = 0; i < total; i++) {
      umap.emplace(i, std::to_string(i));  // template ctor
    }
    check(db, umap);

    std::unordered_multiset<std::string> umset;
    for (int i = 0; i < total; i++) {
      umset.insert(std::to_string(i));
    }
    check(db, umset);

    std::unordered_multimap<int, std::string> ummap;
    for (int i = 0; i < total; i++) {
      ummap.emplace(i % (total / 2),
                    std::to_string(i % (total / 2)));  // template ctor
      // ^^^ testing multiple instances of same key to be sure
    }
    check(db, ummap);

    // D Container adapters
    // std::stack<std::string> stack;
    // for (int i = 0; i < total; i++)
    // {
    //   stack.push(std::to_string(i));
    // }
    // check(db, stack);

    // std::queue<std::string> queue;
    // for (int i = 0; i < total; i++)
    // {
    //   queue.push(std::to_string(i));
    // }
    // check(db, queue);

    // std::priority_queue<std::string> priority_queue;
    // for (int i = 0; i < total; i++)
    // {
    //   priority_queue.push(std::to_string(i));
    // }
    // check(db, priority_queue);

    db->destroy();
  }
}

// TODO get this working. Hidden now so the feature can be finished
TEST_CASE("datatypes-customtypes-memory", "[.][datatypes][customtypes][memory]") {
  // Story:-
  //   [Who]   As an app programmer
  //   [What]  I need to store and retrieve a custom data type
  //   [Value] So I can persist specialist POD classes in GroundUpDB efficiently
  SECTION("datatypes-customtypes-memory") {
    std::string dbname("myemptydb");
    std::unique_ptr<groundupdb::IDatabase> db(
        groundupdb::GroundUpDB::createEmptyDB(dbname));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value
    std::string str("An important string");
    MyCustomType mct(str);

    // groundupdb::HashedValue hv = mct; // conversion operator
    // groundupdb::EncodedValue ev(hv); // conversion ctor

    groundupdb::EncodedValue ev = static_cast<groundupdb::EncodedValue>(mct); // conversion operator

    check(db, ev);

    db->destroy();
  }
}
