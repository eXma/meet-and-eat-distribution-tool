project(mue)
cmake_minimum_required(VERSION 2.8)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb3") # implicit: -g
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=native -fvisibility=hidden") # implicit: -DNDEBUG -O3 

option(PROFILING "PROFILING" OFF)


# Use this for PERF
if (PROFILING)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ggdb3 -fno-omit-frame-pointer")
endif()

# find python
set(CMAKE_PYTHON_VERSIONS 3.4)
set(Python_ADDITIONAL_VERSIONS 3.4)
find_package(PythonLibs 3.4 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

# find boost
set(BOOST_MULTITHREADED ON)
set(BOOST_USE_STATIC_LIBS OFF)
set(BOOST_ADDITIONAL_VERSIONS "1.55" "1.55.0")

find_package(Boost 1.40.0 REQUIRED COMPONENTS python-py34)
include_directories(${Boost_INCLUDE_DIRS})

# subpackages
include_directories(libmue)

add_subdirectory(libmue)
add_subdirectory(pymue)
add_subdirectory(mue)
