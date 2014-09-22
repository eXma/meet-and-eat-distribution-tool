# find files
file(GLOB pymue_SOURCES *.cpp)
file(GLOB pymue_PYTHON *.py)

# build a shared module
add_library(pymue MODULE ${pymue_SOURCES})

# fix the shared object name and link in openmp
set_target_properties(pymue PROPERTIES
  PREFIX "_"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/")

# link dependencies
target_link_libraries(pymue ${PythonLibs_LIBRARIES})
target_link_libraries(pymue ${Boost_PYTHON_LIBRARY})
target_link_libraries(pymue ${Boost_LIBRARIES})
target_link_libraries(pymue libmue)

# install the shared object
install(TARGETS pymue LIBRARY DESTINATION "pymue/")

# install the python files
install(FILES ${pymue_PYTHON} DESTINATION "pymue/")
