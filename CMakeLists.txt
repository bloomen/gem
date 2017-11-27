cmake_minimum_required(VERSION 2.8)
project(gem)
enable_testing()
 
include_directories(src)

set(SOURCES src/test_circular_buffer.cpp src/test_main.cpp)

add_executable(gem_test ${SOURCES})

target_compile_definitions(gem_test PRIVATE UNITTEST)

add_test(gem_test transwarp_test --use-colour no)

if (MSVC)
   set(CMAKE_CXX_FLAGS "/W4 /WX /bigobj /EHsc /wd4503 /wd4996 /wd4702")
else()
   set(CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall -Wextra -Wconversion -Werror")
   target_link_libraries(gem_test ${CMAKE_THREAD_LIBS_INIT})
endif()
