cmake_minimum_required(VERSION 3.12)

add_executable(groundupdb-tests
	dbmanagement-tests.cpp
	hashing-tests.cpp
	keyvalue-tests.cpp
	keyvalue-bug-tests.cpp
	performance-tests.cpp
	query-tests.cpp
	datatypes-tests.cpp
)

include_directories(${groundupdb_SOURCE_DIR})

# This assume highwayhash is a sub-module of this repo..
include_directories(${groundupdb_SOURCE_DIR}/highwayhash)
# if you have built highwayhash as a peer repo, use this line
#include_directories(${groundupdb_SOURCE_DIR}/../highwayhash)

target_link_libraries(groundupdb-tests PRIVATE groundupdb)

target_compile_features(groundupdb-tests PRIVATE cxx_std_17)
