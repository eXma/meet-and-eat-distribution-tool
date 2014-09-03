file(GLOB libmue_SOURCES *.cpp)

add_library(libmue SHARED ${libmue_SOURCES})

set_target_properties(libmue PROPERTIES PREFIX "")

install (TARGETS libmue LIBRARY DESTINATION "lib/")

add_subdirectory(tests)
