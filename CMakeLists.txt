cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

include_directories(/usr/include/dbus-1.0)
include_directories(/usr/lib/x86_64-linux-gnu/dbus-1.0/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# core lib
include_directories(include)
aux_source_directory(src CORE_SRCS)
MESSAGE(STATUS "Core sources: " ${CORE_SRCS})
add_library(dboost STATIC ${CORE_SRCS})	

# test lib 
set(EX1_PATH test/01-basic-scenario) 

# test lib / idl
set(BE_PATH -p ${PROJECT_SOURCE_DIR}/buildtools/idl/backends)
file(GLOB IDLS ${EX1_PATH}/*.idl)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)
set(IFCS "")
foreach(IDL ${IDLS})
  get_filename_component(FILENAME ${IDL} NAME_WE)
  MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
  set(DSTPATH ${CMAKE_CURRENT_BINARY_DIR}/include)  
  add_custom_command(OUTPUT ${DSTPATH}/${FILENAME}.hpp 
                     COMMAND omniidl 
                     ARGS ${BE_PATH} -bcppbackend -C${CMAKE_CURRENT_BINARY_DIR}/include -K ${IDL}
                     COMMENT "Creating ${FILENAME}.hpp")
  list(APPEND IFCS ${DSTPATH}/${FILENAME}.hpp)
endforeach()
add_custom_target(gen-ifcs ALL DEPENDS ${IFCS})

# desired impl
set(DES1_PATH desiredimpl/01-basic-scenario)
MESSAGE(STATUS "DES1_PATH: " ${DES1_PATH})
aux_source_directory(${DES1_PATH} DES1_SRC)
add_library(timer_dboost STATIC ${DES1_SRC})
add_dependencies(timer_dboost gen-ifcs)
target_link_libraries(timer_dboost dboost)
include_directories(${DES1_PATH})

# test lib / main
aux_source_directory(${EX1_PATH} EX1_SRCS)
MESSAGE(STATUS "EX1_SRCS : " ${EX1_SRCS})
add_executable(server-01 ${EX1_SRCS})
target_link_libraries(server-01 dboost timer_dboost dbus-1)
add_dependencies(server-01 dboost timer_dboost)

