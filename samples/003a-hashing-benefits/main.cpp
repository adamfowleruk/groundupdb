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
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>

int main()
{
  // Store 65 000 items in memory of random length
  //  (max 16 letters @ 8 bits per char = 128 bits)
  //  (max 128 letters @ 8 bits per char = 1024 bits)
  // QUIZ: Why not 100 000 above?
  char* buffer;
  int bufferLength = 32;

  const int storeSize = 65000; // Number of records in our 'database'



  // OPTION 1: Store unsorted

  const unsigned short int chosen = (storeSize / 4) * 3 + 1;
  std::string chosenString;
  std::string* stringPtrArray[storeSize]; // array of pointers to strings
  int ci = 0;
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  for (int idx = 0;idx < storeSize;idx++) {
    buffer = new char[bufferLength + 1]; // termination \0
    for (ci = 0;ci < bufferLength;ci++) {
      buffer[ci] = (rand() % 26) + 'a'; // random char from a-z
    }
    buffer[bufferLength] = '\0';
    stringPtrArray[idx] = new std::string(buffer);
    if (chosen == idx) {
      chosenString = *stringPtrArray[idx]; // copy constructor
    }
  }
  // Storage time?
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  std::cout << "Storage: Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
  std::cout << "Storage: Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
  std::cout << "Storage: Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
/*
Storage: Time difference = 34[ms]
Storage: Time difference = 34292[µs]
Storage: Time difference = 34292787[ns]
*/





  // OPTION 1a: Retrieve by key name

  // Pull back a random one by name (akin to a search by the key string in a K-V store)
  begin = std::chrono::steady_clock::now();
  int retrievedIdx = -1;
  for (int idx = 0;idx < storeSize;idx++) {
    if (chosenString == *stringPtrArray[idx]) {
      retrievedIdx = idx;
      break; // end loop early
    }
  }
  end = std::chrono::steady_clock::now();
  std::cout << "Value at 0: " << *stringPtrArray[0] << std::endl;
  std::cout << "Value at 1: " << *stringPtrArray[1] << std::endl;
  std::cout << "Retrieved         : " << chosenString << " at index: " << retrievedIdx << std::endl;
  std::cout << "Retrieval by name : Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
  std::cout << "Retrieval by name : Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
  std::cout << "Retrieval by name : Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
/*
Value at 0: lrfkqyuqfjkxyqvnrtysfrzrmzlygfve
Value at 1: ulqfpdbhlqdqrrcrwdnxeuoqqeklaitg
Retrieved         : oviwymmnaqptldgltxzaoofhohntvctq at index: 48751
Retrieval by name : Time difference = 4[ms]
Retrieval by name : Time difference = 4351[µs]
Retrieval by name : Time difference = 4351301[ns]
*/





  // OPTION 1b: Retrieve by known key index (position)

  // Pull back a random one by index, and record the time taken
  std::string retrieved;
  begin = std::chrono::steady_clock::now();
  retrieved = *stringPtrArray[chosen];
  end = std::chrono::steady_clock::now();
  std::cout << "Retrieved         : " << retrieved << std::endl;
  std::cout << "Retrieval by index: Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
  std::cout << "Retrieval by index: Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
  std::cout << "Retrieval by index: Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
/*
Retrieved         : oviwymmnaqptldgltxzaoofhohntvctq
Retrieval by index: Time difference = 0[ms]
Retrieval by index: Time difference = 0[µs]
Retrieval by index: Time difference = 935[ns]
*/








  // Solution formulation...

  // QUESTION: What is more performant: Fetch by name, or fetch by index?
  //   ANSWER: Index takes 1/263 of the time! It's *MUCH* faster!
  // QUESTION: If we hash the name and use this hash value as the index, could we improve things?
  //   ANSWER: Probably, yes
  //    QUESTION: Could the calculation of the hash take longer than retrieval of the value by name? What does this imply?
  //      ANSWER: Possibly, if not many keys or key names were all short. This means it will be slower than by name! ;o(









  // OPTION 2: Store by the hash of a key in an efficient (O log(n)) unordered map
  // C++11 reference https://www.cplusplus.com/reference/unordered_map/unordered_map/

  // Now do the same for a hash table variant
  // Hash table -> Convert type (string) to a hash number, store data at hash index, and retrieve by hash index
  // Use built in hash map type in C++ for this
  std::unordered_map<std::string,std::string> keyValueStore;
  std::string key;
  std::string randomValue = "wibble"; // doesn't matter - we're test key speed
  begin = std::chrono::steady_clock::now();
  for (int idx = 0;idx < storeSize;idx++) {
    buffer = new char[bufferLength + 1]; // termination \0
    for (ci = 0;ci < bufferLength;ci++) {
      buffer[ci] = (rand() % 26) + 'a'; // random char from a-z
    }
    buffer[bufferLength] = '\0';
    key = std::string(buffer);
    keyValueStore.insert({key,randomValue}); // C++11, creates a std::pair<string,string>
    if (chosen == idx) {
      chosenString = key; // copy constructor
    }
  }
  end = std::chrono::steady_clock::now();
  std::cout << "MapStore: Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
  std::cout << "MapStore: Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
  std::cout << "MapStore: Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
/*
MapStore: Time difference = 103[ms]
MapStore: Time difference = 103129[µs]
MapStore: Time difference = 103129217[ns]
*/





  // OPTION 2: Retrieval time
  begin = std::chrono::steady_clock::now();
  auto kvPair = keyValueStore.find(chosenString);
  std::string retKey = kvPair->first; // first is key
  std::string retValue = kvPair->second; // second is value
  end = std::chrono::steady_clock::now();
  std::cout << "Retrieved key    : " << retKey << " with value: " << retValue << std::endl;
  std::cout << "Retrieve from Map: Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
  std::cout << "Retrieve from Map: Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
  std::cout << "Retrieve from Map: Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
/*
Retrieved key    : yfuhybliokgniztyfuquvlaleaznpcsr with value: wibble
Retrieve from Map: Time difference = 0[ms]
Retrieve from Map: Time difference = 1[µs]
Retrieve from Map: Time difference = 1236[ns]
*/




  // CONCLUSIONS???

  // QUESTION: How did storage speeds compare amongst the two mechanisms?
  //   ANSWER: Hash method is 3 times slower! 103ms vs 34ms. "I thought you said this would be BETTER to use!?!" (well...)
  // QUESTION: How did retrieval speeds compare against the three mechanisms (fetch by name, by index (pre-computed hash), and by hash)?
  //   ANSWER: name: 4351301ns, index: 935ns, hash: 1249ns
  //           i.e. Retrieval was 1/3483 the time vs by name, but 1.33 times longer than a raw index
  //           BUT raw C++ in memory indexes are the quickest you're going to get - seek to memory location
  //            - You can't guarantee a large chunk of contiguous memory is given to your database server as
  //              most Operating Systems randomise memory locations for security, so you can't use raw memory.
  //           CONCLUSION: Hash based map implementations are insanely quick - we should totally use hashes and in-memory unordered maps
}

































// BONUS QUESTION: Why is rand() not random?
//         ANSWER: https://stackoverflow.com/questions/28656004/c-random-doesnt-workreturns-same-value-always
//                 Really, you need to use an implementation that uses modern processors'
//                 secure random instruction set.
