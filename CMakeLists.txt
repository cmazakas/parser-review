cmake_minimum_required(VERSION 3.25)
project(fiona LANGUAGES CXX)

if (${CMAKE_CXX_STANDARD} LESS "17")
  message(FATAL_ERROR "fiona is a C++17 library; current CMAKE_CXX_STANDARD is: ${CMAKE_CXX_STANDARD}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Boost 1.83 REQUIRED)

set(
  fiona_headers

  include/http/response_parser.hpp)

set(
  fiona_sources

  src/response_parser.cpp)

add_library(fiona ${fiona_sources})

target_include_directories(fiona PUBLIC include)

target_link_libraries(
  fiona
  PUBLIC
  Boost::headers
)

target_sources(
  fiona
  PUBLIC FILE_SET HEADERS
  BASE_DIRS include
  FILES ${fiona_headers}
)

target_compile_definitions(fiona PRIVATE FIONA_SOURCE)
if (BUILD_SHARED_LIBS)
  target_compile_definitions(fiona PRIVATE FIONA_DYN_LINK)
endif()

add_library(Fiona::fiona ALIAS fiona)

option(FIONA_BUILD_TESTING "" OFF)
if(FIONA_BUILD_TESTING)
  set_target_properties(fiona PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)
  include(CTest)
  include(FionaTest)
  add_subdirectory(test)
endif()
