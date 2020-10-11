cmake_minimum_required(VERSION 3.12)

add_executable(groundupdb-cli main.cpp)

target_link_libraries(groundupdb-cli PRIVATE groundupdb)

target_compile_features(groundupdb-cli PRIVATE cxx_std_17)

include_directories(${groundupdb_SOURCE_DIR})

install(TARGETS groundupdb-cli
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
)
