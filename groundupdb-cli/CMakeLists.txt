cmake_minimum_required(VERSION 3.12)

add_executable(groundupdb-cli main.cpp)

target_link_libraries(groundupdb-cli PRIVATE groundupdb stdc++fs)

include_directories(${groundupdb_SOURCE_DIR})

install(TARGETS groundupdb-cli
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
)
