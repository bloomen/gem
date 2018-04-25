cmake_minimum_required(VERSION 2.8)
project(gem)
enable_testing()
 
include_directories(src)

set(SOURCES test/main.cpp test/test_circular_buffer.cpp test/test_data_value.cpp)

add_executable(gem_test ${SOURCES})

add_test(gem_test gem_test --use-colour no)

if (MSVC)
   set(CMAKE_CXX_FLAGS "/W4 /bigobj /EHsc /wd4503 /wd4996 /wd4702")
else()
   set(CMAKE_CXX_FLAGS "-std=c++17 -pedantic -Wall -Wextra -Wconversion")
endif()
