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
#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <functional>

// TODO find a way around including the below (its internals)
#include "hashes.h"

namespace groundupdb {

// Name for a byte container whose implementation may change
// in future, but whose semantics will remain the same
using Bytes = std::vector<std::byte>;

template<class T>
class Key {
private:
  Bytes m_data;
public:
  Key(const T& t) : m_data() {
    m_data.reserve(t.size()); 
    std::transform(t.begin(), t.end(), std::back_inserter(m_data),
                   [] (auto c) { return static_cast<std::byte>(c); });
  }
  virtual ~Key() = default;

  const Bytes& data() const {
    return m_data;
  }

  std::size_t length() const {
    return m_data.size();
  }

  bool operator==(const Key<T>& other) const {
    return m_data == other.m_data;
  }

  bool operator!=(const Key<T>& other) const {
    return ! *this==other;
  }
};

class EncodedValue;

/**
 * @brief The HashedValue class is a Value type, intended to be copied cheaply.
 *
 * HashedKey is a HashedValue
 */
class HashedValue {
private:
  bool m_has_value;
  Bytes m_data; // original key data binary representation
  std::size_t m_length; // binary length in bytes
  std::size_t m_hash; // one-way hash of the key binary representation using the server's specified algorithm
public:
  //HashedValue(Bytes data,int length,std::size_t hash);
  HashedValue(const Bytes& data,std::size_t length,std::size_t hash);
  HashedValue();
  template<class T>
  HashedValue(const Key<T>& from)
    : m_has_value(true)
  {
    m_length = from.length();
    //m_data.reserve(m_length);
    //from.copy_to(m_data.begin());

    m_data = from.data(); // copy ctor

    // for (auto& d : from.value()) {
    //   m_data.push_back((std::byte)d);
    // }
    // TODO use correct hasher for current database connection, with correct initialisation settings
    DefaultHash h1{};
    m_hash = h1(from.data());
  }

  /** Copy/move constuctors and operators **/
  //HashedValue(HashedValue& from);
  HashedValue(const HashedValue& from);
  HashedValue(const HashedValue&& from);
  HashedValue& operator=(const HashedValue& other);

  /** Conversion constuctors **/
  HashedValue(const EncodedValue& from);
  HashedValue(EncodedValue&& from);

  /** Standard type convenience constructors **/
  HashedValue(std::string from);

  virtual ~HashedValue() = default;
  const Bytes data() const;
  std::size_t length() const;
  std::size_t hash() const;
  bool hasValue() const;

  // define << and >> operators
  //friend std::ofstream& operator<<(std::ofstream& out, const HashedValue& from);

  // TODO >>
  // TODO assign(iterator,iteratorend) function to load data from disk

  // equals
  bool operator==(const HashedValue& other) const;
  bool operator!=(const HashedValue& other) const;

};

/*
std::ofstream& operator<<(std::ofstream& out, const HashedValue& from) {
  out << from.m_hash << from.m_has_value << from.m_length << from.m_data; // TODO verify this is correct for a char* (m_data)
  return out;
}
*/

using HashedKey = HashedValue; // Alias for semantic ease

using KeySet = std::unique_ptr<std::unordered_set<HashedValue>>;

enum class Type {
  UNKNOWN = 0,
  KEY = 1,
  SET = 2,
  CPP = 3 // TODO determine if we can do this and use refelection, or if we need another way of refering to C++ types
};

std::ostream &operator<<( std::ostream &os, const Type t);
std::istream &operator>>( std::istream &is, Type t);

/**
 * @brief The EncodedValue class is a Value type, intended to be copied cheaply.
 */
class EncodedValue {
private:
  bool m_has_value;
  Type m_type; // internal groundupdb type identifier
  HashedValue m_value; // same internal representation as a HashedKey, so re-using definition
public:
  //EncodedValue(Type type,Bytes data,int length,std::size_t hash): m_has_value(true), m_type(type), m_value(data,length,hash) {}
  EncodedValue(Type type,const Bytes& data,std::size_t length,std::size_t hash): m_has_value(true), m_type(type), m_value(data,length,hash) {}
  EncodedValue() : m_has_value(false), m_type(Type::UNKNOWN), m_value() {}

  /** Copy/move constuctors and operators **/
  //EncodedValue(EncodedValue& from) : m_has_value(from.m_has_value), m_type(from.m_type), m_value(from.m_value) {}
  EncodedValue(const EncodedValue& from) : m_has_value(from.m_has_value), m_type(from.m_type), m_value(from.m_value) {}
  EncodedValue(const EncodedValue&& from) : m_has_value(from.m_has_value), m_type(from.m_type), m_value(std::move(from.m_value)) {
    //std::cout << "EncodedValue::move-ctor" << std::endl;
  }
  EncodedValue& operator=(const EncodedValue& other) {
    m_has_value = other.m_has_value;
    m_type = other.m_type;
    m_value = other.m_value;
    return *this;
  };

  /** Conversion constuctors **/
  EncodedValue(const std::string& from): m_has_value(true), m_type(Type::CPP), m_value(HashedValue{from}) {}

  /** Class methods **/
  Type type() const { return m_type; }
  const Bytes data() const { return m_value.data(); }
  std::size_t length() const { return m_value.length(); }
  std::size_t hash() const { return m_value.hash(); }
  bool hasValue() const { return m_has_value; }

  // define << operator
  //friend std::ofstream& operator<<(std::ofstream& out, const EncodedValue& from);
  // TODO >> operator

  // equals
  bool operator==(const EncodedValue& other) const;
  bool operator!=(const EncodedValue& other) const;
};

/*
std::ofstream& operator<<(std::ofstream& out, const EncodedValue& from) {
  out << from.m_type.c_str() << std::endl;
  out << from.m_has_value;
  if (from.m_has_value) {
    out << from.m_value;
  }
  return out;
}
*/

using Set = std::unique_ptr<std::unordered_set<EncodedValue>>;

} // end namespace

// std::hash support for classes in this file
namespace std
{
    template<>
    struct hash<groundupdb::EncodedValue>
    {
        size_t operator()(const groundupdb::EncodedValue& v) const
        {
          std::size_t hv = 0;
          for (auto& vpart : v.data()) {
            hv = hv ^ (std::to_integer<std::size_t>(vpart) << 1);
          }
          return hv;
        }
    };
    template<>
    struct hash<groundupdb::HashedValue>
    {
        size_t operator()(const groundupdb::HashedValue& v) const
        {
          std::size_t hv = 0;
          for (auto& vpart : v.data()) {
            hv = hv ^ (std::to_integer<std::size_t>(vpart) << 1);
          }
          return hv;
        }
    };
    template<typename T>
    struct hash<groundupdb::Key<T>>
    {
        size_t operator()(const groundupdb::Key<T>& v) const
        {
          std::size_t hv = 0;
          for (auto& vpart : v.data()) {
            hv = hv ^ (std::to_integer<std::size_t>(vpart) << 1);
          }
          return hv;
        }
    };
}


#endif // TYPES_H
