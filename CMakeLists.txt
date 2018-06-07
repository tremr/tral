cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(sample_library VERSION 0.1 DESCRIPTION "tral")

set(SOURCE_LIB src/list.cpp)
add_library(tral STATIC ${SOURCE_LIB})