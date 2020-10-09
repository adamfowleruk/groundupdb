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
#include "hashes.h"
#include "types.h"

namespace groundupdb {

class DefaultHash::Impl {
public:
  Impl() : m_hasher() {}
  Impl(std::uint64_t s1,std::uint64_t s2,std::uint64_t s3,std::uint64_t s4) : m_hasher(s1,s2,s3,s4) {}

  groundupdbext::HighwayHash m_hasher;
};

DefaultHash::DefaultHash()
  : mImpl(std::make_unique<Impl>())
{
  ;
}

DefaultHash::DefaultHash(std::uint64_t s1,std::uint64_t s2,std::uint64_t s3,std::uint64_t s4)
  : mImpl(std::make_unique<Impl>(s1,s2,s3,s4))
{
  ;
}

DefaultHash::~DefaultHash()
{
  ;
}

std::size_t
DefaultHash::operator() (const std::string& s) const noexcept {
  return mImpl->m_hasher(s);
}

std::size_t
DefaultHash::operator() (const char* data,std::size_t length) const noexcept {
  return mImpl->m_hasher(data,length);
}

std::size_t
DefaultHash::operator() (const HashedValue& s) const noexcept {
  return mImpl->m_hasher(s);
}

std::size_t
DefaultHash::operator() (const EncodedValue& s) const noexcept {
  return mImpl->m_hasher(s);
}

std::size_t
DefaultHash::operator() (const std::vector<std::byte>& bytes) const noexcept {
  return mImpl->m_hasher(bytes);
}

} // end namespace
