#add_executable(mue test.cpp)

#target_link_libraries(mue libmue)

#set_target_properties(mue PROPERTIES
#  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/")

#install(TARGETS mue RUNTIME DESTINATION ".")
install(FILES mue.py DESTINATION ".")
