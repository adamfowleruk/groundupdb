cmake_minimum_required(VERSION 3.12)

project(groundupdb VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

add_subdirectory(groundupdb)
add_subdirectory(groundupdb-cli)
add_subdirectory(groundupdb-tests)