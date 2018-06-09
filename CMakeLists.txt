cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(tral VERSION 0.1 DESCRIPTION "tral")

include_directories( ${CMAKE_SOURCE_DIR}/tral )

set(SOURCE_LIB ${SOURCE_LIB} src/list.cpp)
set(SOURCE_LIB ${SOURCE_LIB} src/IndexedString.cpp)
set(SOURCE_LIB ${SOURCE_LIB} src/IndexedContainer.cpp)
set(SOURCE_LIB ${SOURCE_LIB} src/DataSource.cpp)

add_library(tral STATIC ${SOURCE_LIB})