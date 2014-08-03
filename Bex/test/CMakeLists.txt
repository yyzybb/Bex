cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_COMPILER)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(boost_include ${CMAKE_SOURCE_DIR}/../../boost/include)
set(boost_lib ${CMAKE_SOURCE_DIR}/../../boost/lib)
set(bex_lib ${CMAKE_SOURCE_DIR}/../../_output/Bex)
set(bex_include ${CMAKE_SOURCE_DIR}/../src)

project(test)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../../_output/${PROJECT_NAME})
include_directories(${CMAKE_SOURCE_DIR}/../test)
include_directories(${CMAKE_SOURCE_DIR}/../test/main)
include_directories(${boost_include})
link_directories(${boost_lib})
include_directories(${bex_include})
link_directories(${bex_lib})

file(GLOB_RECURSE SRC_LIST "*.cpp" )
add_executable(test ${SRC_LIST})

#g++ -o test.exe main/TestMain.cpp stream/TestStream.cpp -I"../src" -I"../../boost/include" -std=c++11 "../../boost/lib/libboost_system-mgw48-mt-s-1_55.a" "../../boost/lib/libboost_unit_test_framework-mgw48-mt-s-1_55.a" "../../boost/lib/libboost_thread-mgw48-mt-s-1_55.a" "../../boost/lib/libboost_locale-mgw48-mt-s-1_55.a" -I"main"
