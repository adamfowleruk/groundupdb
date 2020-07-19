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
#ifndef HIGHWAYHASH_H
#define HIGHWAYHASH_H

#include <string>
#include "highwayhash/highwayhash.h"

namespace groundupdbext {

using namespace highwayhash;

class HighwayHash {
public:
  HighwayHash();
  HighwayHash(std::uint64_t s1,std::uint64_t s2,std::uint64_t s3,std::uint64_t s4);
  ~HighwayHash();

  std::size_t operator() (std::string const& s) const noexcept;
private:
  const HHKey m_key HH_ALIGNAS(64);
  HighwayHashCatT<HH_TARGET>* m_hh;
  HHResult64* m_result;
};

}

#endif // HIGHWAYHASH_H