project(mue)
cmake_minimum_required(VERSION 2.8)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0") # implicit: -g
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=native") # implicit: -DNDEBUG -O3 

# find python
set(CMAKE_PYTHON_VERSIONS 2.6)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

# find boost
set(BOOST_MULTITHREADED ON)
set(BOOST_USE_STATIC_LIBS OFF)
set(BOOST_ADDITIONAL_VERSIONS "1.42" "1.42.0")

find_package(Boost 1.40.0 REQUIRED COMPONENTS python)
include_directories(${Boost_INCLUDE_DIRS})

# subpackages
include_directories(libmue)

add_subdirectory(libmue)
add_subdirectory(pymue)
add_subdirectory(mue)
