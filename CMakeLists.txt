cmake_minimum_required(VERSION 3.25)

project(ITA C)

set(CMAKE_C_STANDARD 11)

set(LIB_DIRECTORY libs)
set(SRC_DIRECTORY src)
set(SRC_FILES ${SRC_DIRECTORY}/main.c ${SRC_DIRECTORY}/utils/utils.c)



add_executable(ITA ${SRC_FILES})

add_library(libstemmer STATIC IMPORTED)
set_target_properties(libstemmer PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIRECTORY}/libstemmer_c-2.2.0/libstemmer.a)
set_target_properties(libstemmer PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIRECTORY}/libstemmer_c-2.2.0/include)

add_library(libset STATIC IMPORTED)
set_target_properties(libset PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIRECTORY}/set/libset.a)
include_directories(PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIRECTORY}/set/src/)

message(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ITA libstemmer)
target_link_libraries(ITA libset)