add_executable(
  hello_test
  vector.cpp
  )

target_link_libraries(
  hello_test
  PUBLIC geodatastruct
  GTest::gtest_main
  )

gtest_discover_tests(hello_test)
