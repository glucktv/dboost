cmake_minimum_required(VERSION 2.8)

set(SRCS timer_impl.cpp main.cpp) 
set(BE_PATH -p ../../buildtools/idl/backends)
set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

file(GLOB IDLS *.idl)
set(IFCS "")
foreach(IDL ${IDLS})
  get_filename_component(FILENAME ${IDL} NAME_WE)
  add_custom_command(OUTPUT ${FILENAME}.hpp 
                     COMMAND omniidl ${BE_PATH} -bcppbackend -K ${IDL}
                     COMMENT "Creating ${FILENAME}.hpp")
  list(APPEND IFCS ${FILENAME}.hpp)
endforeach()

include_directories(../../include)
include_directories(/usr/include/dbus-1.0)
include_directories(/usr/lib/x86_64-linux-gnu/dbus-1.0/include)

include_directories(../../desiredimpl/01-basic-scenario)

add_custom_target(gen-ifcs ALL DEPENDS ${IFCS})
add_executable(server-01 timer_impl.cpp main.cpp)
