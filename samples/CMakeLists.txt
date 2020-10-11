include_directories(${groundupdb_SOURCE_DIR})
include_directories(${groundupdb_SOURCE_DIR}/../highwayhash)

add_executable(003a-hashing-benefits 003a-hashing-benefits/main.cpp)
set_property(TARGET 003a-hashing-benefits PROPERTY FOLDER Samples)
target_compile_features(003a-hashing-benefits PRIVATE cxx_std_17)
install(TARGETS 003a-hashing-benefits
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
)

add_executable(008a-types 008a-types/main.cpp)
set_property(TARGET 008a-types PROPERTY FOLDER Samples)
target_compile_features(008a-types PRIVATE cxx_std_17)
target_link_libraries(008a-types PRIVATE groundupdb)
install(TARGETS 008a-types
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
)