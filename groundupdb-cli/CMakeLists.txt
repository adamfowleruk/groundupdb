cmake_minimum_required(VERSION 3.12)

add_executable(groundupdb-cli main.cpp)

target_link_libraries(groundupdb-cli PRIVATE groundupdb)

install(TARGETS groundupdb-cli)