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
#ifndef QUERY_H
#define QUERY_H

#include "types.h"

#include <string>
#include <memory>
#include <unordered_set>

namespace groundupdb {

// Tagging class for now
class Query {
public:
  Query() = default;
  virtual ~Query() = default;
private:
};

class IQueryResult {
public:
  IQueryResult() = default;
  virtual ~IQueryResult() = default;

  virtual const KeySet& recordKeys() = 0;
};

// MARK: Query Implementation Types


// default empty query matches all data
class EmptyQuery : public Query {
public:
  EmptyQuery() = default;
  virtual ~EmptyQuery() = default;
};

// Parent bucket query
class BucketQuery : public Query {
public:
  BucketQuery(std::string& bucket);
  virtual ~BucketQuery();

  virtual std::string bucket() const;
private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};

}
#endif // QUERY_H
