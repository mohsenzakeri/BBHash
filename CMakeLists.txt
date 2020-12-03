cmake_minimum_required(VERSION 3.0)

include_directories(include)

project(bbhash)

set(bbhash_lib_srcs src/BitVector.cpp src/RankSupport.cpp src/BBHash.cpp src/xxhash.c)
add_library(bbhash STATIC ${bbhash_lib_srcs})
target_include_directories(bbhash PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")

#add_executable(main src/main.cpp)
add_executable(main src/main.cpp)

#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include/libdivsufsort"
#                    "${CMAKE_CURRENT_BINARY_DIR}/../include/libdivsufsort")
#link_directories("${CMAKE_CURRENT_BINARY_DIR}/../include/libdivsufsortlib")

# Link the executable and the library together
target_link_libraries(main bbhash)
