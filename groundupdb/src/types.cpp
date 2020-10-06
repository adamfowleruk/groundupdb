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
#include "types.h"
#include "extensions/highwayhash.h"

namespace groundupdb {

HashedValue::HashedValue(const Bytes& data,std::size_t length,std::size_t hash)
  : m_has_value(true),
    m_data(data),
    m_length(length),
    m_hash(hash)
{
  ;
}

HashedValue::HashedValue()
  : m_has_value(false),
    m_data(),
    m_length(0),
    m_hash(0)
{
  ;
}

/** Copy/move constuctors and operators **/
HashedValue::HashedValue(HashedValue& from)
  : m_has_value(from.m_has_value),
    m_data(from.m_data),
    m_length(from.m_length),
    m_hash(from.m_hash)
{
  ;
}

HashedValue::HashedValue(const HashedValue& from)
  : m_has_value(from.m_has_value),
    m_data(from.m_data),
    m_length(from.m_length),
    m_hash(from.m_hash)
{
  ;
}

HashedValue::HashedValue(const HashedValue&& from)
  : m_has_value(from.m_has_value),
    m_data(from.m_data),
    m_length(from.m_length),
    m_hash(from.m_hash)
{
  ;
}

HashedValue&
HashedValue::operator=(const HashedValue& other)
{
  m_has_value = other.m_has_value;
  m_length = other.m_length;
  m_data = other.m_data;
  m_hash = other.m_hash;
  return *this;
}

/** Conversion constuctors **/
HashedValue::HashedValue(const EncodedValue& from)
  : m_has_value(from.hasValue()),
    m_data(from.data()),
    m_length(from.length()),
    m_hash(from.hash())
{
  ;
}

HashedValue::HashedValue(std::string from)
  : m_has_value(true),
    m_data()
{
  m_length = from.length();
  m_data.reserve(m_length);
  for (auto& c : from) {
    m_data.push_back((std::byte)c);
  }
  // TODO use correct hasher for current database connection, with correct initialisation settings
  DefaultHash h1{1,2,3,4};
  m_hash = h1(from);
}

const Bytes
HashedValue::data() const { return m_data; }

std::size_t
HashedValue::length() const { return m_length; }

std::size_t
HashedValue::hash() const { return m_hash; }

bool
HashedValue::hasValue() const { return m_has_value; }

bool
HashedValue::operator==(const HashedValue& other) const
{
  // compare hash first as generally it will be faster most often
  if (m_hash != other.m_hash || m_has_value != other.m_has_value || m_length != other.m_length) {
    return false;
  }
  // only do a data wise comparison if you must (highly, highly unlikely - requires hash collision)
  //return 0 == std::strcmp(m_data,other.m_data);
  return m_data == other.m_data;
}

bool
HashedValue::operator!=(const HashedValue& other) const
{
  return !(*this==other);
}

bool
EncodedValue::operator==(const EncodedValue& other) const
{
  return m_has_value==other.m_has_value && m_type == other.m_type && (m_value == other.m_value);
}

bool
EncodedValue::operator!=(const EncodedValue& other) const
{
  return !(*this==other);
}

std::ostream &
operator<<(std::ostream &os, const Type t) {
  os << (int)t;
  return os;
}

std::istream &operator>>(std::istream &is, Type t) {
  int tint;
  is >> tint;
  t = (Type)tint;
  return is;
}

}
