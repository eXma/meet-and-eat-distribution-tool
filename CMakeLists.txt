project(mue)
cmake_minimum_required(VERSION 2.8)

enable_testing()

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
