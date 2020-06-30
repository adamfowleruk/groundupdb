cmake_minimum_required(VERSION 3.12)

set(HEADERS 
	include/groundupdb/groundupdb.h
	include/groundupdb/database.h
	include/groundupdb/extensions/extdatabase.h
)

add_library(groundupdb 
	${HEADERS}
	src/database.cpp
	src/groundupdb.cpp
)
set_target_properties(groundupdb PROPERTIES PUBLIC_HEADER "${HEADERS}")


target_include_directories(groundupdb 
	PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS groundupdb 
	EXPORT groundupdb
	LIBRARY
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/groundupdb
)

install(EXPORT groundupdb 
	NAMESPACE groundupdb::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/groundupdb
)