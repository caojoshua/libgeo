add_executable(geotest)

target_link_libraries(
  geotest
  PUBLIC geodatastruct
  PUBLIC geo
  GTest::gtest_main
  )

add_subdirectory(data_structure)
add_subdirectory(geometry)

gtest_discover_tests(geotest)
