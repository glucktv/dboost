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

function(build_example NAME)

    # test lib 
    set(EX1_PATH test/${NAME}) 
    
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
    add_custom_target(gen-ifcs-${NAME} ALL DEPENDS ${IFCS})
    
    # desired impl
    set(DES1_PATH desiredimpl/${NAME})
    MESSAGE(STATUS "DES1_PATH: " ${DES1_PATH})
    aux_source_directory(${DES1_PATH} DES1_SRC)
    add_library(dboost_test_${NAME} STATIC ${DES1_SRC})
    add_dependencies(dboost_test_${NAME} gen-ifcs-${NAME})
    target_link_libraries(dboost_test_${NAME} dboost)
    include_directories(${DES1_PATH})
    
    # test lib / server
    file(GLOB EX1S_IMPL_SRCS "${EX1_PATH}/*_impl.cpp")
    set(EX1S_SRCS ${EX1S_IMPL_SRCS} ${EX1_PATH}/server.cpp)
    add_executable(${NAME}-server ${EX1S_SRCS})
    target_link_libraries(${NAME}-server dboost dboost_test_${NAME} dbus-1)
    add_dependencies(${NAME}-server dboost dboost_test_${NAME})
    
    # test lib / client
    set(EX1C_SRCS ${EX1_PATH}/client.cpp)
    add_executable(${NAME}-client ${EX1C_SRCS})
    target_link_libraries(${NAME}-client dboost dboost_test_${NAME} dbus-1)
    add_dependencies(${NAME}-client dboost dboost_test_${NAME})

endfunction(build_example)

build_example(01-basic-scenario)
build_example(05-struct-passing)
