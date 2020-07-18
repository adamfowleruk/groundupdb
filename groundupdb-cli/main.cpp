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
#include "cxxopts.hpp"
#include "groundupdb/groundupdb.h"
using namespace std;

using namespace groundupdb;

cxxopts::Options options("groundupdb-cli","CLI for GroundUpDB");

void printUsage() {
  cout << "Whoops bad config!" << endl;
}

// Incorporating https://github.com/jarro2783/cxxopts as a header only library for options parsing
int main(int argc, char* argv[]) {
    // 1. Grab command line parameters and determine mode
    options.add_options()
      ("c,create", "Create a DB")
      ("d,destroy", "Destroy a DB")
      ("s,set", "Set a key in a DB")
      ("g,get", "Get a key from a DB")
      ("n,name","Database name (required)", cxxopts::value<std::string>())
      ("k,key","Key to set/get", cxxopts::value<std::string>())
      ("v,value","Value to set", cxxopts::value<std::string>())
    ;
    auto result = options.parse(argc, argv);

    if (result.count("c") == 1) {
      if (result.count("n") == 0) {
        cout << "You must specify a database name with -n <name>" << endl;
        printUsage();
        return 1;
      }
      // create database
      std::string dbname(result["n"].as<std::string>());
      std::unique_ptr<groundupdb::IDatabase> db(GroundUpDB::createEmptyDB(dbname));
      return 0;
    }
    if (result.count("d") == 1) {
        if (result.count("n") == 0) {
            cout << "You must specify a database name with -n <name>" << endl;
            printUsage();
            return 1;
        }
        // destroy database
        std::string dbname(result["n"].as<std::string>());
        std::unique_ptr<groundupdb::IDatabase> db(GroundUpDB::loadDB(dbname));
        db->destroy();
        return 0;
    }
    if (result.count("s") == 1) {
        if (result.count("n") == 0) {
            cout << "You must specify a database name with -n <name>" << endl;
            printUsage();
            return 1;
        }
        if (result.count("k") == 0) {
            cout << "You must specify a key to set with -k <name>" << endl;
            printUsage();
            return 1;
        }
        if (result.count("v") == 0) {
            cout << "You must specify a value to set with -v <value>" << endl;
            printUsage();
            return 1;
        }
        // Set key value in database
        std::string dbname(result["n"].as<std::string>());
        std::string k(result["k"].as<std::string>());
        std::string v(result["v"].as<std::string>());
        std::unique_ptr<groundupdb::IDatabase> db(GroundUpDB::loadDB(dbname));
        db->setKeyValue(k,v);
        return 0;
    }
    if (result.count("g") == 1) {
        if (result.count("n") == 0) {
            cout << "You must specify a database name with -n <name>" << endl;
            printUsage();
            return 1;
        }
        if (result.count("k") == 0) {
            cout << "You must specify a key to set with -k <name>" << endl;
            printUsage();
            return 1;
        }
        // Get key value from database
        std::string dbname(result["n"].as<std::string>());
        std::string k(result["k"].as<std::string>());
        std::unique_ptr<groundupdb::IDatabase> db(GroundUpDB::loadDB(dbname));
        cout << db->getKeyValue(k) << endl;
        return 0;
    }

    cout << "No command specified" << endl;
    printUsage();
    return 1;
}
