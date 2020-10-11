cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

set(HEADERS 
	include/groundupdb.h
	include/database.h
	include/hashes.h
	include/query.h
	include/is_container.h
	include/types.h
	include/extensions/extdatabase.h
	include/extensions/extquery.h
	include/extensions/highwayhash.h
)

add_library(groundupdb 
	${HEADERS}
	src/database.cpp
	src/filekeyvaluestore.cpp
	src/groundupdb.cpp
	src/hashes.cpp
	src/highwayhash.cpp
	src/memorykeyvaluestore.cpp
	src/query.cpp
	src/types.cpp
)
set_target_properties(groundupdb PROPERTIES PUBLIC_HEADER "${HEADERS}")

# NB: this assumes google's highwayhash is available as a sub-module of this repo..
include_directories(${groundupdb_SOURCE_DIR}/highwayhash)
# use this version if you have highwayhash as a peer module outside this repo..
#include_directories(${groundupdb_SOURCE_DIR}/../highwayhash)

target_include_directories(groundupdb 
	PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_compile_features(groundupdb PRIVATE cxx_std_17)

# NB: This is here to ensure binaries that link us also link stdc++fs for non-Apple targets
# https://github.com/OpenRCT2/OpenRCT2/pull/10522
if(NOT (APPLE OR MSVC) )
target_link_libraries(groundupdb PRIVATE stdc++fs)
endif(NOT (APPLE OR MSVC) )

install(TARGETS groundupdb 
    EXPORT groundupdb
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/groundupdb
)

install(EXPORT groundupdb 
	NAMESPACE groundupdb::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/groundupdb
)
