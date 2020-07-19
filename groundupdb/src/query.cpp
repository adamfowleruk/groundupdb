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
#include "query.h"
#include "extensions/extquery.h"

#include <string>

using namespace groundupdb;
using namespace groundupdbext;

class BucketQuery::Impl {
public:
  Impl(std::string& bucket);
  ~Impl() = default;
  std::string m_bucket;
};

BucketQuery::Impl::Impl(std::string& bucket)
  : m_bucket(bucket)
{
  ;
}

BucketQuery::BucketQuery(std::string& bucket)
  : mImpl(std::make_unique<Impl>(bucket))
{
  ;
}

BucketQuery::~BucketQuery()
{
  ;
}

std::string
BucketQuery::bucket() const {
  return mImpl->m_bucket;
}

DefaultQueryResult::DefaultQueryResult()
  : m_recordKeys()
{
  ;
}

DefaultQueryResult::DefaultQueryResult(std::unique_ptr<std::unordered_set<std::string>> recordKeys)
  : m_recordKeys(std::move(recordKeys))
{
  ;
}

const std::unique_ptr<std::unordered_set<std::string>>
DefaultQueryResult::recordKeys() {
  return std::move(m_recordKeys);
}
