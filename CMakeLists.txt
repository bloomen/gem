cmake_minimum_required(VERSION 2.8)
project(gem)
enable_testing()
 
include_directories(src)

set(SOURCES
	src/gem/circular_buffer.h
	src/gem/datastore.h
	src/gem/hashmap.h
	src/gem/resource_pool.h
	src/gem/spinlock.h
	src/gem/type.h
	test/main.cpp
	test/test_circular_buffer.cpp
	test/test_datastore.cpp
	test/test_hashmap.cpp
	test/test_resource_pool.cpp
	test/test_type.cpp
	)

find_package(Threads)

add_executable(gem_test ${SOURCES})

add_test(gem_test gem_test --use-colour no)

if (MSVC)
   set(CMAKE_CXX_FLAGS "/std:c++17 /W4 /bigobj /EHsc /wd4503 /wd4996 /wd4702")
else()
   set(CMAKE_CXX_FLAGS "-std=c++17 -pedantic -Wall -Wextra -Wconversion")
   if (CMAKE_COMPILER_IS_GNUCC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
   endif()
   target_link_libraries(gem_test ${CMAKE_THREAD_LIBS_INIT})
endif()
