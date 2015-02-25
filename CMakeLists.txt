cmake_minimum_required(VERSION 2.8)

aux_source_directory(src SOURCES)
include_directories(include)
set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

include_directories(/usr/include/dbus-1.0)
include_directories(/usr/lib/x86_64-linux-gnu/dbus-1.0/include)

add_library(dboost STATIC ${SOURCES})
