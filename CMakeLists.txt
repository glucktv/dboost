cmake_minimum_required(VERSION 2.7.8)

set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

include_directories(/usr/include/dbus-1.0)
include_directories(/usr/lib/x86_64-linux-gnu/dbus-1.0/include)
include_directories(${PROJECT_SOURCE_DIR}/include)


# core lib
aux_source_directory(src CORE_SRCS)
MESSAGE(STATUS "Core sources: " ${CORE_SRCS})
add_library(dboost STATIC ${CORE_SRCS})	

function(build_example NAME)

    # test lib 
    set(EX_PATH test/${NAME}) 
    set(DSTPATH ${CMAKE_CURRENT_BINARY_DIR}/${NAME})  
    
    # test lib / idl
    set(BE_PATH -p ${PROJECT_SOURCE_DIR}/buildtools/idl/backends)
    file(GLOB IDLS ${EX_PATH}/*.idl)
    file(MAKE_DIRECTORY ${DSTPATH})
    set(IFCS "")
    foreach(IDL ${IDLS})
      get_filename_component(FILENAME ${IDL} NAME_WE)
      add_custom_command(OUTPUT ${DSTPATH}/${FILENAME}.hpp ${DSTPATH}/${FILENAME}_proxy.hpp ${DSTPATH}/${FILENAME}_adaptor.hpp ${DSTPATH}/${FILENAME}_proxy.cpp ${DSTPATH}/${FILENAME}_adaptor.cpp
                         COMMAND omniidl 
                         ARGS ${BE_PATH} -bcxxbackend -C${DSTPATH} -K ${IDL}
                         COMMENT "Processing interface '${FILENAME}'")
      list(APPEND IFCS ${DSTPATH}/${FILENAME}.hpp)
    endforeach()
    add_custom_target(gen-ifcs-${NAME} ALL DEPENDS ${IFCS})
    
    # automatically generated code 
    set(DES_PATH ./${NAME})
    add_library(dboost_test_${NAME} STATIC 
	        ${DSTPATH}/${FILENAME}_proxy.cpp 
		${DSTPATH}/${FILENAME}_adaptor.cpp)
    add_dependencies(dboost_test_${NAME} gen-ifcs-${NAME})
    target_link_libraries(dboost_test_${NAME} dboost)
    
    # test lib / server
    file(GLOB EXS_IMPL_SRCS "${EX_PATH}/*_impl.cpp")
    set(EXS_SRCS ${EXS_IMPL_SRCS} ${EX_PATH}/server.cpp)
    add_executable(${NAME}-server ${EXS_SRCS})
    target_link_libraries(${NAME}-server dboost dboost_test_${NAME} dbus-1 boost_system)
    add_dependencies(${NAME}-server dboost dboost_test_${NAME})
    
    # test lib / client
    set(EXC_SRCS ${EX_PATH}/client.cpp)
    add_executable(${NAME}-client ${EXC_SRCS})
    target_link_libraries(${NAME}-client dboost dboost_test_${NAME} dbus-1 boost_system dboost_test_${NAME})
    add_dependencies(${NAME}-client dboost dboost_test_${NAME})
    set_property(TARGET dboost_test_${NAME} ${NAME}-server ${NAME}-client 
	    APPEND PROPERTY INCLUDE_DIRECTORIES ${DSTPATH} ${PROJECT_SOURCE_DIR}/${DES_PATH})

endfunction(build_example)

build_example(01-basic-scenario)
build_example(05-struct-passing)
#build_example(02-observer)
build_example(03-exceptions)
