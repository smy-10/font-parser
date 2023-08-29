cmake_minimum_required(VERSION 3.3)

project(font_parser)

set(CMAKE_CXX_STANDARD 11)

set(BUILD_SHARED_LIB on)

#ENV VAR INIT

if (LIB_DIR_NAME)
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/${LIB_DIR_NAME}")
else ()
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
endif ()

set(LIB_NAME "${PROJECT_NAME}")
set(LIB_OUTPUT_DIR "${OUTPUT_DIR}/libs")

set(3RD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party")
set(3RD_PARTY_INCLUDES_DIR "${3RD_PARTY_DIR}/includes")
set(3RD_PARTY_LIBS_DIR "${3RD_PARTY_DIR}/libs")
set(3RD_PARTY_BIN_DIR "${3RD_PARTY_DIR}/bin")

include_directories(${3RD_PARTY_INCLUDES_DIR})
include_directories(${3RD_PARTY_DIR}/opt_includes)
link_directories(${3RD_PARTY_LIBS_DIR})


add_subdirectory(src)
