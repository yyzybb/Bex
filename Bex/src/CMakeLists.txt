cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_COMPILER)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(boost_include ${CMAKE_SOURCE_DIR}/../../../library/Boost/include)
set(boost_lib ${CMAKE_SOURCE_DIR}/../../../library/Boost/lib)

project(Bex)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../../_output/${PROJECT_NAME})
include_directories(${CMAKE_SOURCE_DIR}/../src)
include_directories(${boost_include})
link_directories(${boost_lib})

file(GLOB_RECURSE SRC_LIST "*.cpp" )
add_library(Bex STATIC ${SRC_LIST})

