add_executable(geometry_test)

target_link_libraries(
  geometry_test
  PUBLIC geo
  GTest::gtest_main
  )

add_subdirectory(structure)

gtest_discover_tests(geometry_test)
