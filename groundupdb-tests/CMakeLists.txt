cmake_minimum_required(VERSION 3.12)

add_executable(groundupdb-tests
	dbmanagement-tests.cpp
	keyvalue-tests.cpp
	performance-tests.cpp
)

target_link_libraries(groundupdb-tests PRIVATE groundupdb)