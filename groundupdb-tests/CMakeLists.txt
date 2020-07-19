cmake_minimum_required(VERSION 3.12)

add_executable(groundupdb-tests
	dbmanagement-tests.cpp
	hashing-tests.cpp
	keyvalue-tests.cpp
	performance-tests.cpp
	query-tests.cpp
)

include_directories(${groundupdb_SOURCE_DIR})

include_directories(${groundupdb_SOURCE_DIR}/../highwayhash)

target_link_libraries(groundupdb-tests PRIVATE groundupdb)