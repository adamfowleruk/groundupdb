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
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <functional>
#include <numeric>

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

/** STL deduction traits **/
template <typename T>
struct has_const_iterator
{
private:
  typedef char yes;
  typedef struct
  {
    char array[2];
  } no;

  template <typename C>
  static yes test(typename C::const_iterator *);
  template <typename C>
  static no test(...);

public:
  static const bool value = sizeof(test<T>(0)) == sizeof(yes);
  typedef T type;
};

template <typename T>
struct has_begin_end
{
  template <typename C>
  static char (&f(typename std::enable_if<
                  std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
                               typename C::const_iterator (C::*)() const>::value,
                  void>::type *))[1];

  template <typename C>
  static char (&f(...))[2];

  template <typename C>
  static char (&g(typename std::enable_if<
                  std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
                               typename C::const_iterator (C::*)() const>::value,
                  void>::type *))[1];

  template <typename C>
  static char (&g(...))[2];

  static bool const beg_value = sizeof(f<T>(0)) == 1;
  static bool const end_value = sizeof(g<T>(0)) == 1;
};

template <typename T>
struct is_container : 
  std::integral_constant<bool, 
    has_const_iterator<T>::value && 
    has_begin_end<T>::beg_value && 
    has_begin_end<T>::end_value>
{
};

// template <typename T>
// struct has_keyed_begin_end
// {
//   template <typename C>
//   static char (&f(typename std::enable_if<
//                   std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
//                                typename C::key_type (C::*)() const>::value,
//                   void>::type *))[1];

//   template <typename C>
//   static char (&f(...))[2];

//   template <typename C>
//   static char (&g(typename std::enable_if<
//                   std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
//                                typename C::key_type (C::*)() const>::value,
//                   void>::type *))[1];

//   template <typename C>
//   static char (&g(...))[2];

//   static bool const beg_value = sizeof(f<T>(0)) == 1;
//   static bool const end_value = sizeof(g<T>(0)) == 1;
// };

// template <typename T>
// struct is_keyed_container : 
//   std::integral_constant<bool, 
//     has_const_iterator<T>::value && 
//     has_keyed_begin_end<T>::beg_value && 
//     has_keyed_begin_end<T>::end_value>
// {
// };

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
  /**
 * Accept any basic type. Templated to minimise coding.
 * Good basic reference on techniques used here: 
 * https://www.internalpointers.com/post/quick-primer-type-traits-modern-cpp
 */
  template <typename VT>
  HashedValue(VT from) : m_has_value(true), m_data(), m_length(0), m_hash(0)
  {
    // first remove reference
    auto v = std::decay_t<VT>(from);
    // second check if its a basic type and convert to bytes
    if constexpr(std::is_same_v<std::string, decltype(v)>) {
      //std::cout << "DECLTYPE std::string" << std::endl;
      m_length = from.length();
      m_data.reserve(m_length);
      std::transform(from.begin(), from.end(), std::back_inserter(m_data),
                     [](auto c) { return static_cast<std::byte>(c); });
    } else if constexpr (std::is_same_v<char *, decltype(v)>) {
      //std::cout << "DECLTYPE char ptr" << std::endl;
      std::string s(v);
      m_length = s.length();
      m_data.reserve(m_length);
      std::transform(s.begin(), s.end(), std::back_inserter(m_data),
                     [](auto c) { return static_cast<std::byte>(c); });
    } else if constexpr (std::is_same_v<const char*, decltype(v)>) {
      //std::cout << "DECLTYPE char array" << std::endl;
      std::string s(v);
      m_length = s.length();
      m_data.reserve(m_length);
      std::transform(s.begin(), s.end(), std::back_inserter(m_data),
                     [](auto c) { return static_cast<std::byte>(c); });
    } else if constexpr(std::numeric_limits<VT>::is_integer) {
      //std::cout << "DECLTYPE numeric" << std::endl;
      m_length = sizeof(v) / sizeof(std::byte);
      auto vcopy = v; // copy ready for modification
      m_data.reserve(m_length);
      for (auto i = std::size_t(0); i < m_length; i++)
      {
        // NOTE This implements little-endian conversion TODO do we want this?
        m_data.push_back(static_cast<std::byte>((vcopy & 0xFF)));
        vcopy = vcopy >> sizeof(std::byte);
      }
    } else if constexpr(std::is_floating_point_v<VT>) {
      //std::cout << "DECLTYPE float" << std::endl;
      m_length = sizeof(v) / sizeof(std::byte);
      auto vcopy = v; // copy ready for modification
      unsigned int asInt = *((int *)&vcopy);
      m_data.reserve(m_length);
      for (auto i = std::size_t(0); i < m_length; i++)
      {
        // NOTE This implements little-endian conversion TODO do we want this?
        m_data.push_back(static_cast<std::byte>((asInt & 0xFF)));
        asInt = asInt >> sizeof(std::byte);
      }
    } else if constexpr (std::is_same_v<Bytes, decltype(v)>) {
      m_length = from.size();
      m_data = std::move(from);
    } else if constexpr (is_container<decltype(v)>::value) {
      // Convert contents to EncodedValue and serialise it
      // TODO in future this will preserve its runtime type
      Bytes data; 
      // ^^^ can't reserve yet - may be dynamic type in container 
      //     (We MUST do a deep copy)
      for (auto iter = v.begin();iter != v.end();++iter) {
        HashedValue hv(*iter); 
        // ^^^ uses template functions, so a little recursion here
        Bytes t = hv.data();
        data.insert(std::end(data),t.begin(),t.end());
        // https://stackoverflow.com/questions/2551775/appending-a-vector-to-a-vector
      }
      // TODO append EncodedValue(CPP,decltype<VT>,...,data) instead
      m_data.insert(std::end(m_data),data.begin(),data.end());
      m_length = m_data.size(); // can only know after inserts
    // } else if constexpr (is_keyed_container<decltype(v)>::value) {
    //   std::cout << "DECLTYPE keyed container" << std::endl;
    } else {
      throw std::runtime_error(typeid(v).name());
      // TODO we don't support it, fire off a compiler warning
      //static_assert(false, "Must be a supported type, or convertible to std::vector<std::byte>!");
    }
    // TODO use correct hasher for current database connection, with correct initialisation settings
    DefaultHash h1{1, 2, 3, 4};
    m_hash = h1(m_data);
  }

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
  template<typename VT>
  EncodedValue(const VT &from) : m_has_value(true), m_type(Type::CPP), m_value(HashedValue{from}) {}

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
