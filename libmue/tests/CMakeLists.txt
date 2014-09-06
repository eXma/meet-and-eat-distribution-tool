set(CXXTEST_TESTGEN_ARGS
  "--xunit-printer" "--fog-parser" "--have-std"
  "--have-eh" "--longlong=long long" "--world=libmue")

find_package(CxxTest REQUIRED)

enable_testing()

set(libmue_TESTS test_seen_table.h test_guest_tuple_iterator.h test_teams.h test_distances.h test_calculation.h test_guest_permutation_iterator.h)
#file(GLOB libmue_TESTS RELATIVE . *.h)

get_target_property(libmue_INCLUDE_DIRS libmue INCLUDE_DIRECTORIES)
include_directories(${libmue_INCLUDE_DIRS})

if(CXXTEST_FOUND)
  include_directories(${CXXTEST_INCLUDE_DIR})

  file(GLOB libmue_unittest_SOURCES *.cpp)
  add_library(libmue_unittest SHARED ${libmue_unittest_SOURCES})

  foreach(test ${libmue_TESTS})
    get_filename_component(name ${test} NAME_WE)
    CXXTEST_ADD_TEST(unittests-libmue-${name} libmue_unittests_${name}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${test})
    target_link_libraries(unittests-libmue-${name} libmue libmue_unittest)
  endforeach(test)
endif(CXXTEST_FOUND)

