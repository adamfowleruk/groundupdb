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
#ifndef HASHES_H
#define HASHES_H

#include "types.h"
#include <string>
#include <memory>

namespace groundupdb {

// forward declarations
class HashedValue;
class EncodedValue;

class DefaultHash {
public:
  DefaultHash();
  DefaultHash(std::uint64_t s1,std::uint64_t s2,std::uint64_t s3,std::uint64_t s4);
  ~DefaultHash();

  std::size_t operator() (const HashedValue& s) const noexcept;
  std::size_t operator() (const EncodedValue& s) const noexcept;
  std::size_t operator() (const std::string& s) const noexcept;
  std::size_t operator() (const std::vector<std::byte>& bytes) const noexcept;
  std::size_t operator() (const char* data,std::size_t length) const noexcept;

  class Impl;
private:
  std::unique_ptr<Impl> mImpl;
};

}
#endif // HASHES_H
