file(GLOB libmue_SOURCES *.cpp)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-exceptions -fno-non-call-exceptions")

add_library(libmue SHARED ${libmue_SOURCES})

set_target_properties(libmue PROPERTIES PREFIX "")

install (TARGETS libmue LIBRARY DESTINATION "lib/")

add_subdirectory(tests)
