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
#include "extensions/highwayhash.h"

namespace groundupdbext {

using namespace highwayhash;

HighwayHash::HighwayHash()
  : m_key{1,2,3,4},
    m_hh(new HighwayHashCatT<HH_TARGET>(m_key)),
    m_result(new HHResult64())
{
  ;
}

HighwayHash::HighwayHash(std::uint64_t s1,std::uint64_t s2,std::uint64_t s3,std::uint64_t s4)
  : m_key{s1,s2,s3,s4},
    m_hh(new HighwayHashCatT<HH_TARGET>(m_key)),
    m_result(new HHResult64())
{
  ;
}

HighwayHash::~HighwayHash() {
  //delete m_hh;
  //delete m_result; // for windows - why does this help???
}

std::size_t
HighwayHash::operator() (std::string const& s) const noexcept {
  m_hh->Reset(m_key);
  m_hh->Append(s.c_str(), s.length());
  m_hh->Finalize(m_result);
  return *m_result;
}

std::size_t
HighwayHash::operator() (const char* data,std::size_t length) const noexcept {
  m_hh->Reset(m_key);
  m_hh->Append(data, length);
  m_hh->Finalize(m_result);
  return *m_result;
}

std::size_t
HighwayHash::operator() (const groundupdb::HashedValue& data) const noexcept {
  return data.hash();
  // TODO ensure the hash has the same basis as that required by this class
}

std::size_t
HighwayHash::operator() (const groundupdb::EncodedValue& data) const noexcept {
  return data.hash();
  // TODO ensure the hash has the same basis as that required by this class
}

std::size_t
HighwayHash::operator() (const groundupdb::Bytes& data) const noexcept {
  m_hh->Reset(m_key);
  for (auto iter = data.begin();iter != data.end();++iter) {
    m_hh->Append((const char*)&*iter,sizeof(*iter));
  }
  m_hh->Finalize(m_result);
  return *m_result;
}

}
