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
#ifndef IS_CONTAINER_H
#define IS_CONTAINER_H

#include <valarray>
#include <type_traits>
#include <tuple>

namespace groundupdb {

template <typename T>
struct is_pair {
  typedef typename std::decay_t<T> test_type;

  template <typename A>
  static constexpr bool test(A* pt, decltype(pt->first)* = nullptr,
                             decltype(pt->second)* = nullptr) {
    typedef typename A::first_type first_type;
    typedef typename A::second_type second_type;

    return std::is_same_v<A, std::pair<first_type, second_type>>;
  }

  template <typename A>
  static constexpr bool test(...) {
    return false;
  }
  static const bool value = test<test_type>(nullptr);
};

template <typename T>
struct is_container {
  typedef typename std::decay_t<T> test_type;

  template <typename A>
  static constexpr bool test(A* pt, A const* cpt = nullptr,
                             decltype(pt->begin())* = nullptr,
                             decltype(pt->end())* = nullptr,
                             decltype(cpt->begin())* = nullptr,
                             decltype(cpt->end())* = nullptr) {
    typedef typename A::iterator iterator;
    typedef typename A::const_iterator const_iterator;
    typedef typename A::value_type value_type;
    return std::is_same<decltype(pt->begin()), iterator>::value &&
           std::is_same<decltype(pt->end()), iterator>::value &&
           std::is_same<decltype(cpt->begin()), const_iterator>::value &&
           std::is_same<decltype(cpt->end()), const_iterator>::value &&
           !is_pair<decltype(*pt->begin())>::value;
  }

  template <typename A>
  static constexpr bool test(...) {
    return false;
  }

  static const bool value = test<test_type>(nullptr);
};

template <typename T, std::size_t N>
struct is_container<T[N]> : std::true_type {};

template <std::size_t N>
struct is_container<char[N]> : std::false_type {};

template <typename T>
struct is_container<std::valarray<T>> : std::true_type {};

template <typename... Args>
struct is_container<std::tuple<Args...>> : std::true_type {};

template <typename T>
struct is_keyed_container {
  typedef typename std::decay_t<T> test_type;

  template <typename A>
  static constexpr bool test(A* pt, A const* cpt = nullptr,
                             decltype(pt->begin())* = nullptr,
                             decltype(pt->end())* = nullptr,
                             decltype(cpt->begin())* = nullptr,
                             decltype(cpt->end())* = nullptr) {
    typedef typename A::iterator iterator;
    typedef typename A::const_iterator const_iterator;
    typedef typename A::value_type value_type;
    return std::is_same<decltype(pt->begin()), iterator>::value &&
           std::is_same<decltype(pt->end()), iterator>::value &&
           std::is_same<decltype(cpt->begin()), const_iterator>::value &&
           std::is_same<decltype(cpt->end()), const_iterator>::value &&
           is_pair<decltype(*pt->begin())>::value;
  }

  template <typename A>
  static constexpr bool test(...) {
    return false;
  }

  static const bool value = test<test_type>(nullptr);
};

}  // namespace groundupdb

#endif  // IS_CONTAINER_H
