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
#ifndef EXTQUERY_H
#define EXTQUERY_H

#include "../query.h"

namespace groundupdbext {

using namespace groundupdb;

class DefaultQueryResult: public IQueryResult {
public:
  DefaultQueryResult();
  DefaultQueryResult(KeySet&& recordKeys);
  DefaultQueryResult(Set&& recordKeys);
  virtual ~DefaultQueryResult() = default;

  const KeySet& recordKeys();
private:
  KeySet m_recordKeys;
};

}

#endif // EXTQUERY_H
